//----------------------------------------------------------------------------------------------------------------------------------

#if 0
The channel offset system needs more attention because it is not correct and need to figure out what the values need to be
No idea what the designers of this thing had in mind, but it is utter crap. It would have been far simpler if the FPGA just returned
the conversion data as is. The position on the display is simple to do in software. For the trigger level simple calculations
could have been used to determine the actual ADC level.
  
Some scaling per input sensitivity setting is normal calibration but needs user interaction and calibrated voltage levels at the input.

#endif

//----------------------------------------------------------------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>

#include <linux/loop.h>

#include <sys/ioctl.h>

#include "signal_generator.h"

#include "stub_functions.h"
#include "ff.h"
#include "diskio.h"
#include "scope_functions.h"
#include "variables.h"

//----------------------------------------------------------------------------------------------------------------------------------

extern int quit_scopeprocessing_thread_on_zero;

//----------------------------------------------------------------------------------------------------------------------------------
//Defines for handling the FPGA

#define FPGA_COMMAND_WRITE     0x06
#define FPGA_COMMAND_READ      0x04
#define FPGA_DATA_WRITE        0x02
#define FPGA_DATA_READ         0x00

#define FPGA_DATA_ERROR        0xFF

//Parameter storage handling
#define PARAM_MODE_READ          0
#define PARAM_MODE_WRITE         1

#define PARAM_STATE_NOT_INIT     0
#define PARAM_STATE_INIT         1

//----------------------------------------------------------------------------------------------------------------------------------

typedef struct tagFPGA_DATA                FPGA_DATA;

//----------------------------------------------------------------------------------------------------------------------------------
//Data for FPGA handling
struct tagFPGA_DATA
{
  uint8        current_command;
  uint16       read_count;
  const uint8 *read_ptr;
  uint16       write_count;
  uint8       *write_ptr;
  
  //Data for parameter storage system
  uint8   param_state;
  uint8   param_crypt;
  uint8   param_mode;
  uint8   param_id;
  uint8   param_length;
  uint8   param_data[7];
  
  uint8   param_0x0B_data;
  
  int16   param_0x12_low;
  int16   param_0x12_high;
  
  uint8   cmd0x14count[2];
};

//----------------------------------------------------------------------------------------------------------------------------------

FILE *sd_emu_file = NULL;

int loop_device_open = 0;

//----------------------------------------------------------------------------------------------------------------------------------

FPGA_DATA fpga_data;

//----------------------------------------------------------------------------------------------------------------------------------

void sys_clock_init(void)
{
}

//----------------------------------------------------------------------------------------------------------------------------------

void arm32_icache_enable(void)
{
}

//----------------------------------------------------------------------------------------------------------------------------------

void arm32_dcache_enable(void)
{
}

//----------------------------------------------------------------------------------------------------------------------------------

void arm32_interrupt_enable(void)
{
}

//----------------------------------------------------------------------------------------------------------------------------------

void timer0_setup(void)
{
  timer0ticks = 0;
}

//----------------------------------------------------------------------------------------------------------------------------------

uint32 timer0_get_ticks(void)
{
  return(timer0ticks);
}

//----------------------------------------------------------------------------------------------------------------------------------

void timer0_delay(uint32 timeout)
{
  //Calculate the timeout ticks
  timeout += timer0ticks;
  
  //While technically prone to error on timer ticks overflow the scope has to be running for >49 days before it occurs
  //Wait until time has past
  while(timer0ticks < timeout);
}

//----------------------------------------------------------------------------------------------------------------------------------

void power_interrupt_init(void)
{
}

//----------------------------------------------------------------------------------------------------------------------------------

void battery_check_init(void)
{
}

//----------------------------------------------------------------------------------------------------------------------------------

void battery_check_status(void)
{
  scopesettings.batterycharging = 1;

  scopesettings.batterychargelevel = 20;
  
  //Display the level on the screen
  scope_battery_status();
}

//----------------------------------------------------------------------------------------------------------------------------------

void sys_spi_flash_init(void)
{
}

//----------------------------------------------------------------------------------------------------------------------------------

void sys_spi_flash_read(int addr, unsigned char *buffer, int length)
{
  FILE *fp = fopen("scope_flash_memory.bin", "rb");
  
  if(fp)
  {
    fseek(fp, addr, SEEK_SET);
    
    fread(buffer, 1, length, fp);
    
    fclose(fp);
  }
}

//----------------------------------------------------------------------------------------------------------------------------------

void sys_spi_flash_write(int addr, unsigned char *buffer, int length)
{
  FILE *fp = fopen("scope_flash_memory.bin", "wb");
  
  if(fp)
  {
    fseek(fp, addr, SEEK_SET);
    
    fwrite(buffer, 1, length, fp);
    
    fclose(fp);
  }
}

//----------------------------------------------------------------------------------------------------------------------------------

void fpga_init(void)
{
}

//----------------------------------------------------------------------------------------------------------------------------------

void fpga_exit(void)
{
}

//----------------------------------------------------------------------------------------------------------------------------------
//Static data
const uint8 tp_coord_reg[2] = { 0x81, 0x50 };

const uint8 param_status_byte = 0x01;


const uint8 scope_ch1_long_data[] = 
{
  0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28,
  0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68,
  0xB8, 0xB8, 0xB8, 0xB8, 0xB8, 0xB8, 0xB8, 0xB8, 0xB8, 0xB8,
  0xC1, 0xC1, 0xC1, 0xC1, 0xC1, 0xC1, 0xC1, 0xC1, 0xC1, 0xC1,
};

uint32 ch1_long_idx = 0;


#define NUMBER_OF_SAMPLES            10000
#define HALF_NUMBER_OF_SAMPLES       (NUMBER_OF_SAMPLES / 2)

#define TRIGGER_FOUND_NEW_BUFFER     0
#define TRIGGER_FOUND_NOT_YET        1
#define TRIGGER_FOUND_YES            2


const uint32 timebase_interval_settings[24] =
{
  2000000, 1000000, 500000,     //200ms/div, 100ms/div and 50ms/div  use the 2KSa/s setting
   200000,  100000,  50000,     //20ms/div, 10ms/div, 5ms/div        use their respective settings
    20000,   10000,   5000,     //2ms/div, 1ms/div, 500us/div        use their respective settings
     2000,    1000,    500,     //200us/div, 100us/div, 50us/div     use their respective settings
      200,     100,     50,     //20us/div, 10us/div, 5us/div        use their respective settings
       20,      10,      5,     //2us/div, 1us/div, 500ns/div        use their respective settings
        5,       5,      5,     //200ns/div, 100ns/div, 50ns/div     use the 200MSa/s setting
        5,       5,      5      //20ns/div, 10ns/div, 5ns/div        use the 200MSa/s setting
};


const uint32 stub_sample_rate_settings[18] =
{
  200000000,     //200MSa/s
  100000000,     //100MSa/s
   50000000,     // 50MSa/s
   20000000,     // 20MSa/s
   10000000,     // 10MSa/s
    5000000,     //  5MSa/s
    2000000,     //  2MSa/s
    1000000,     //  1MSa/s
     500000,     //500KSa/s
     200000,     //200KSa/s
     100000,     //100KSa/s
      50000,     // 50KSa/s
      20000,     // 20KSa/s
      10000,     // 10KSa/s
       5000,     //  5KSa/s
       2000,     //  2KSa/s
       1000,     //  1KSa/s
        500,     // 500Sa/s
};


uint8 cmd0F_input;

uint8 cmd40_response = 40;

uint8 no_data_response = 0;

double channel1_samples[NUMBER_OF_SAMPLES];
double channel2_samples[NUMBER_OF_SAMPLES];

uint8 channel1_signal_data_1[HALF_NUMBER_OF_SAMPLES];
uint8 channel1_signal_data_2[HALF_NUMBER_OF_SAMPLES];

uint8 channel2_signal_data_1[HALF_NUMBER_OF_SAMPLES];
uint8 channel2_signal_data_2[HALF_NUMBER_OF_SAMPLES];

double channel1_scaler = 272.108843537414934;
double channel2_scaler = 272.108843537414934;

double channel1_offset = 128;
double channel2_offset = 128;


uint32 trigger_channel = 0;
uint32 trigger_edge    = 0;
uint32 trigger_mode    = 0;

uint32 trigger_found   = TRIGGER_FOUND_NEW_BUFFER;

uint8  trigger_level   = 0;

double sample_rate_set = 200000000.0;

uint32 sample_index = 1500;
uint32 sample_count = 1500;

//----------------------------------------------------------------------------------------------------------------------------------

uint8 calculatesample(double sample, double scaler, double offset, int adccalib)
{
  sample = (sample * scaler) + offset + adccalib;
  
  if(sample < 0)
  {
    sample = 0;
  }
  else if(sample > 255)
  {
    sample = 255;
  }
  
  return((uint8)sample);
}

//----------------------------------------------------------------------------------------------------------------------------------

void getsamples(void)
{
  int i;
  double *sptr1;
  double *sptr2;
  
  //Get a bunch of samples for both channels
  //Needs the sample interval to be set based on the time base setting
  signalgeneratorgetsamples(0, channel1_samples, NUMBER_OF_SAMPLES, sample_rate_set);
  signalgeneratorgetsamples(1, channel2_samples, NUMBER_OF_SAMPLES, sample_rate_set);

  //Need some DC filter system here when channel on AC
  
  
  //Point to the first samples
  sptr1 = channel1_samples;
  sptr2 = channel2_samples;

  //Separate and convert all the samples into the two adc buffers per channel
  for(i=0;i<HALF_NUMBER_OF_SAMPLES;i++)
  {
    //Adjust the samples based on a scaler and an offset. Also put in a calibration error between the two converters.
    channel1_signal_data_2[i] = calculatesample(*sptr1++, channel1_scaler, channel1_offset, 4);
    channel1_signal_data_1[i] = calculatesample(*sptr1++, channel1_scaler, channel1_offset, 0);

    channel2_signal_data_2[i] = calculatesample(*sptr2++, channel2_scaler, channel2_offset, -5);
    channel2_signal_data_1[i] = calculatesample(*sptr2++, channel2_scaler, channel2_offset, 0);
  }
      
  //No trigger yet when data is fresh from new capture
  trigger_found = TRIGGER_FOUND_NEW_BUFFER;
}

//----------------------------------------------------------------------------------------------------------------------------------

uint32 findtriggerpoint(void)
{
  uint8 *buffer;  
  uint32 index;
  uint32 count;
  uint32 sample1;
  uint32 sample2;
  
  //Select the trace buffer to process based on the trigger channel
  if(trigger_channel == 0)
  {
    //Channel 1 buffer
    buffer = channel1_signal_data_1;
  }
  else
  {
    //Channel 2 buffer
    buffer = channel2_signal_data_1;
  }
  
  //Set a starting point for checking on trigger. Half way a normal buffer
  index = 750;
  count = HALF_NUMBER_OF_SAMPLES - 750;

  while(count--)
  {
    sample1 = buffer[index];
    sample2 = buffer[index + 1];

    if(((trigger_edge == 0) && (sample1 < trigger_level) && (sample2 > trigger_level)) ||
       ((trigger_edge == 1) && (sample1 > trigger_level) && (sample2 < trigger_level)))
    {
      //Set the current index as trigger point
      sample_index = index - (sample_count / 2);
      
      //Done with checking and trigger found so signal that
      return(1);
    }
    
    //Select next sample to check
    index++;
  }
  
  //No trigger found so signal this
  return(0);
}


//----------------------------------------------------------------------------------------------------------------------------------

void fpga_write_cmd(uint8 command)
{
  int i;

  //Set the new command as current
  fpga_data.current_command = command;

  //Decide on which action to take
  switch(fpga_data.current_command)
  {
    case 0x0A:
      //Get a bunch of samples for both channels
      getsamples();
      break;
     
    //Start sampling command when data is 1
    case 0x0F:
      fpga_data.write_ptr = &cmd0F_input;
      fpga_data.write_count = 1;
      break;
      
    case 0x14:
      fpga_data.read_count = 2;
      fpga_data.read_ptr = fpga_data.cmd0x14count;
      break;

    case 0x20:  //Channel 1
      fpga_data.read_count = sample_count;
      fpga_data.read_ptr = &channel1_signal_data_1[sample_index];
      break;

    case 0x21:
      fpga_data.read_count = sample_count;
      fpga_data.read_ptr = &channel1_signal_data_2[sample_index];
      break;

    case 0x22:
      fpga_data.read_count = sample_count;
      fpga_data.read_ptr = &channel2_signal_data_1[sample_index];
      break;

    case 0x23:
      fpga_data.read_count = sample_count;
      fpga_data.read_ptr = &channel2_signal_data_2[sample_index];
      break;

    case 0x38:
      //Set brightness
      break;

    case 0x41:
      //Read touch panel coordinates register address
      fpga_data.read_count = 2;
      fpga_data.read_ptr = tp_coord_reg;
      break;

    case 0x64:
      fpga_data.param_mode = PARAM_MODE_READ;
      break;

    case 0x65:
      fpga_data.param_mode = PARAM_MODE_WRITE;
      break;

    case 0x66:
      //Start the process based on the mode
      if(fpga_data.param_mode == PARAM_MODE_READ)
      {
        //Check if this is the first read after start up
        if(fpga_data.param_state == PARAM_STATE_NOT_INIT)
        {
          //Set it for reading
          fpga_data.param_data[3] = fpga_data.param_crypt;

          //Signal parameter storage has been initialized
          fpga_data.param_state = PARAM_STATE_INIT;
        }
        else
        {
          switch(fpga_data.param_id)
          {
            case 0x0B:
              //This one depends on data coming from the scope
              fpga_data.param_data[3] = 0;
              fpga_data.param_data[4] = 0;
              fpga_data.param_data[5] = 0;
              
              switch(fpga_data.param_0x0B_data)
              {
                case 0:
                  fpga_data.param_data[6] = 0xAD;
                  break;
                  
                case 1:
                  fpga_data.param_data[6] = 0xAF;
                  break;
                  
                case 2:
                case 3:
                  fpga_data.param_data[6] = 0xB4;
                  break;
                  
                default:
                  fpga_data.param_data[6] = 0xB8;
                  break;
              }
              break;
              
            case 0x0C:
              fpga_data.param_data[3] = 0;
              fpga_data.param_data[4] = 0;
              fpga_data.param_data[5] = 0;
              fpga_data.param_data[6] = 0x20;
              break;
              
            case 0x0D:
              fpga_data.param_data[3] = 0;
              fpga_data.param_data[4] = 0;
              fpga_data.param_data[5] = 0;
              fpga_data.param_data[6] = 0x22;
              break;
              
            case 0x12:
            {
              int offset = (fpga_data.param_0x12_high - fpga_data.param_0x12_low) * 100;
              
              fpga_data.param_data[3] = offset >> 24;
              fpga_data.param_data[4] = offset >> 16;
              fpga_data.param_data[5] = offset >> 8;
              fpga_data.param_data[6] = offset;
              break;
            }
          }

          //Add the crypt byte to the data
          fpga_data.param_data[0] = fpga_data.param_crypt;

          //Invert it for the crypting
          fpga_data.param_crypt = ~fpga_data.param_crypt;

          //Decide what size descriptor needs to be returned
          if(fpga_data.param_data[3])
          {
            //More than 24 bits used then use 0xAA
            fpga_data.param_data[1] = 0xAA;
          }
          else if(fpga_data.param_data[4])
          {
            //More than 16 bits but less than 24 bits used then use 0xA5
            fpga_data.param_data[1] = 0xA5;
          }
          else if(fpga_data.param_data[5])
          {
            //More than 8 bits but less than 16 bits used then use 0x5A
            fpga_data.param_data[1] = 0x5A;
          }
          else
          {
            //8 bits or less used then use 0x55
            fpga_data.param_data[1] = 0x55;
          }

          //Calculate the checksum
          fpga_data.param_data[2] = fpga_data.param_crypt + fpga_data.param_data[1] + fpga_data.param_data[3] + fpga_data.param_data[4] + fpga_data.param_data[5] + fpga_data.param_data[6];

          //Crypt the data
          for(i=1;i<7;i++)
          {
            fpga_data.param_data[i] ^= fpga_data.param_crypt;
          }  
        }
      }
      else
      {
        //Get the parameter id for file handling
        fpga_data.param_id = (fpga_data.param_data[1] ^ fpga_data.param_crypt) >> 2;
        
        fpga_data.param_length = (fpga_data.param_data[1] ^ fpga_data.param_crypt) & 0x03;
        
        switch(fpga_data.param_id)
        {
          case 0x0B:
            //No checks, just get the data for now
            fpga_data.param_0x0B_data = fpga_data.param_data[6] ^ fpga_data.param_crypt;
            break;
            
          case 0x12:
            //Inefficient but quick fix
            if(fpga_data.param_length == 3)
            {
              fpga_data.param_0x12_low  = (fpga_data.param_data[6] ^ fpga_data.param_crypt) | ((fpga_data.param_data[5] ^ fpga_data.param_crypt) << 8);
              fpga_data.param_0x12_high = (fpga_data.param_data[4] ^ fpga_data.param_crypt) | ((fpga_data.param_data[3] ^ fpga_data.param_crypt) << 8);
            }
            else if(fpga_data.param_length == 2)
            {
              fpga_data.param_0x12_low  = (fpga_data.param_data[6] ^ fpga_data.param_crypt) | ((fpga_data.param_data[5] ^ fpga_data.param_crypt) << 8);
              fpga_data.param_0x12_high = (fpga_data.param_data[4] ^ fpga_data.param_crypt);
            }
            else if(fpga_data.param_length == 1)
            {
              fpga_data.param_0x12_low  = (fpga_data.param_data[6] ^ fpga_data.param_crypt) | ((fpga_data.param_data[5] ^ fpga_data.param_crypt) << 8);
              fpga_data.param_0x12_high = 0;
            }
            else
            {
              fpga_data.param_0x12_low  = (fpga_data.param_data[6] ^ fpga_data.param_crypt);
              fpga_data.param_0x12_high = 0;
            }
            break;
        }
      }
      break;
  
    case 0x68:
    case 0x69:
    case 0x6A:
    case 0x6B:
    case 0x6C:
    case 0x6D:
    case 0x6E:
      if(fpga_data.param_mode == PARAM_MODE_READ)
      {
        fpga_data.read_ptr = &fpga_data.param_data[fpga_data.current_command - 0x68];
        fpga_data.read_count = 1;
      }
      else
      {
        fpga_data.write_ptr = &fpga_data.param_data[fpga_data.current_command - 0x68];
        fpga_data.write_count = 1;
      }
      break;
  }
}

//----------------------------------------------------------------------------------------------------------------------------------

uint8 fpga_read_cmd(void)
{
}

//----------------------------------------------------------------------------------------------------------------------------------

void fpga_write_byte(uint8 data)
{
  //Store the data in the target register if needed
  if(fpga_data.write_count)
  {
    *fpga_data.write_ptr = data;

    fpga_data.write_ptr++;
    fpga_data.write_count--;
    
    //Handle the input data when all bytes are in
    if(fpga_data.write_count == 0)
    {
      switch(fpga_data.current_command)
      {
        case 0x0F:
          if(cmd0F_input == 0)
          {
            //Freeze the signal generator to allow for proper sampling
            signalgeneratorfreeze();
          }
          else
          {
            //Unfreeze the signal generator
            signalgeneratorunfreeze();
          }
          break;
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------------------

uint8 fpga_read_byte(void)
{
  uint8 retval = 0xFF;
  double *sptr1;
  double *sptr2;
  int i;

  switch(fpga_data.current_command)
  {
    default:
      if(fpga_data.read_count)
      {
        retval = *fpga_data.read_ptr;

        fpga_data.read_ptr++;
        fpga_data.read_count--;
      }
      break;
      
    case 0x05:
      retval = 1;
      break;
      
    case 0x0A:
      //Check on what mode the trigger system is set to
      if(trigger_mode == 0)
      {
        //Auto mode needs to find a trigger point, but when not found just use fixed point in the buffer and signal done
        if(findtriggerpoint() == 0)
        {
          sample_index = 1500;
        }
        
        retval = 1;
      }
      else
      {
        //For normal and single triggering the system needs to find a valid trigger. If not signal not done and load more samples on next call
        if(trigger_found == TRIGGER_FOUND_NEW_BUFFER)
        {
          //When a new buffer is loaded scan for a trigger in it
          retval = findtriggerpoint();
          
          //Check if the trigger is found
          if(retval == 1)
          {
            //Trigger has been found so done for now
            trigger_found = TRIGGER_FOUND_YES;
          }
          else
          {
            //No trigger found so signal this
            trigger_found = TRIGGER_FOUND_NOT_YET;
          }
        }
        else if(trigger_found == TRIGGER_FOUND_NOT_YET)
        {
          //No trigger found in previous data so read some more data and look for a trigger in it
          //Not a proper solution in respect to pre trigger data but the simplest way for now
          
          //Get a bunch of samples for both channels
          getsamples();
          
          //Signal still looking for trigger
          retval = 0;
        }
      }
      
      //Make sure sample index is within proper limits
      if(sample_index > (HALF_NUMBER_OF_SAMPLES - sample_count))
      {
        sample_index = (HALF_NUMBER_OF_SAMPLES - sample_count);
      }
      break;
      
    case 0x67:
      retval = 1;
      break;
  }

  return(retval);
}

//----------------------------------------------------------------------------------------------------------------------------------

void fpga_write_short(uint16 data)
{
  fpga_write_byte((data >> 8) & 0xFF);
  fpga_write_byte(data & 0xFF);
}

//----------------------------------------------------------------------------------------------------------------------------------

uint16 fpga_read_short(void)
{
  return((fpga_read_byte() << 8) | fpga_read_byte());
}

//----------------------------------------------------------------------------------------------------------------------------------

void fpga_write_int(uint32 data)
{
  fpga_write_byte((data >> 24) & 0xFF);
  fpga_write_byte((data >> 16) & 0xFF);
  fpga_write_byte((data >> 8) & 0xFF);
  fpga_write_byte(data & 0xFF);
}

//----------------------------------------------------------------------------------------------------------------------------------

void fpga_set_backlight_brightness(uint16 brightness)
{
}

//----------------------------------------------------------------------------------------------------------------------------------

void fpga_set_translated_brightness(void)
{
}

//----------------------------------------------------------------------------------------------------------------------------------

uint16 fpga_get_version(void)
{
}

//----------------------------------------------------------------------------------------------------------------------------------

void fpga_check_ready(void)
{
}

//----------------------------------------------------------------------------------------------------------------------------------

void fpga_enable_system(void)
{
}

//----------------------------------------------------------------------------------------------------------------------------------

void fpga_set_channel_enable(PCHANNELSETTINGS settings)
{
}

//----------------------------------------------------------------------------------------------------------------------------------

void fpga_set_channel_coupling(PCHANNELSETTINGS settings)
{
}

//----------------------------------------------------------------------------------------------------------------------------------

void fpga_set_channel_voltperdiv(PCHANNELSETTINGS settings)
{
  double *scaler;
  
  if(settings->voltperdivcommand == 0x33)
  {
    scaler = &channel1_scaler;
  }
  else
  {
    scaler = &channel2_scaler;
  }
  
  //Check if setting in range of what is allowed
  switch(settings->voltperdiv)
  {
    case 0:
      *scaler = 5.746347581618066;
      break;

    case 1:
      *scaler = 11.52832674571805;
      break;

    case 2:
      *scaler = 27.870680044593088;
      break;

    case 3:
      *scaler = 55.741360089186154;
      break;

    case 4:
      *scaler = 137.151634847487382;
      break;

    default:
    case 5:
      *scaler = 272.108843537414934;
      break;
  }
}

//----------------------------------------------------------------------------------------------------------------------------------

void fpga_set_channel_offset(PCHANNELSETTINGS settings)
{
  uint32 offset;
  double *channel_offset;

  if(settings->offsetcommand == 0x32)
  {
    channel_offset = &channel1_offset;
  }
  else
  {
    channel_offset = &channel2_offset;
  }
  
  //Write the offset data
  *channel_offset = settings->dc_calibration_offset[settings->voltperdiv] * 0.170666666666667;
}

//----------------------------------------------------------------------------------------------------------------------------------

void fpga_set_sample_rate(uint32 samplerate)
{
  //Set the sample interval based on the time base
  if(samplerate < (sizeof(stub_sample_rate_settings) / sizeof(uint32)))
  {
    //Table settings ranges from setting 0 (200mS/div) to 23 (5nS/div)
    sample_rate_set = stub_sample_rate_settings[samplerate];
  }
  
  //New setup always allows 1500 samples
  sample_count = 1500;
}

//----------------------------------------------------------------------------------------------------------------------------------

void fpga_set_trigger_channel(void)
{
  trigger_channel = scopesettings.triggerchannel;
}

//----------------------------------------------------------------------------------------------------------------------------------

void fpga_set_trigger_edge(void)
{
  trigger_edge = scopesettings.triggeredge;
}

//----------------------------------------------------------------------------------------------------------------------------------

void fpga_swap_trigger_channel(void)
{
  //Check if channel 1 is disabled or channel 2 is enabled
  if((scopesettings.channel1.enable == 0) || (scopesettings.channel2.enable))
  {
    //Check if both channels are disabled
    if(scopesettings.channel2.enable == 0)
    {
      //If so do nothing
      return;
    }
    
    //Else check if channel 1 is enabled
    if(scopesettings.channel1.enable)
    {
      //Assume the correct setting is already in the FPGA and do nothing
      return;
    }
    
    //Only channel 2 enabled so set it as source
    scopesettings.triggerchannel = 1;
  }
  else
  {
    //Only channel 1 enabled so set that as source
    scopesettings.triggerchannel = 0;
  }
  
  trigger_channel = scopesettings.triggerchannel;
}

//----------------------------------------------------------------------------------------------------------------------------------

void fpga_set_trigger_level(void)
{
  uint32 voltperdiv;
  uint32 traceposition;
  int32  level;
  
  //The trace position on screen equals to ADC reading 128
  //The trigger vertical position is relative to the trace position.
  //The screen position values are volt/div related, so conversion to ADC level is needed
  
  //Check which channel is used for triggering
  if(scopesettings.triggerchannel == 0)
  {
    //Channel 1, so use its data
    voltperdiv    = scopesettings.channel1.voltperdiv;
    traceposition = scopesettings.channel1.traceposition;
  }
  else
  {
    //Channel 2, so use its data
    voltperdiv    = scopesettings.channel2.voltperdiv;
    traceposition = scopesettings.channel2.traceposition;
  }
  
  
  //The difference between the two positions determines the level offset on 128, but it needs to be scaled back first
  level = ((((int32)traceposition - (int32)scopesettings.triggerverticalposition) * 4194304) / (41954 * signal_adjusters[voltperdiv])) + 128;

#if 0  
  level = scopesettings.triggerverticalposition - traceposition;
  
  level = (level * 4194304) / (41954 * signal_adjusters[voltperdiv]);
  
  level = level + 128;
#endif
  
  //Limit on extremes
  if(level < 0)
  {
    level = 0;
  }
  else if(level > 255)
  {
    level = 255;
  }
  
  //Save it for usage by FPGA emulator code
  trigger_level = level;
}

//----------------------------------------------------------------------------------------------------------------------------------

void fpga_set_trigger_mode(void)
{
  trigger_mode = scopesettings.triggermode;
}

//----------------------------------------------------------------------------------------------------------------------------------

void fpga_set_long_timebase(void)
{
}

//----------------------------------------------------------------------------------------------------------------------------------

void fpga_set_time_base(void)
{
}

//----------------------------------------------------------------------------------------------------------------------------------

uint16 fpga_prepare_for_transfer(void)
{
}

//----------------------------------------------------------------------------------------------------------------------------------

void fpga_read_sample_data(PCHANNELSETTINGS settings, uint32 triggerpoint)
{
  //Clear min and max and average for new calculations
  settings->min     = 0x7FFFFFFF;
  settings->max     = 0;
  settings->average = 0;
  
  //Send command 0x1F to the FPGA followed by the translated data returned from command 0x14
  fpga_write_cmd(0x1F);
  fpga_write_short(triggerpoint);
  
  //Send a command for getting ADC1 trace data from the FPGA
  fpga_write_cmd(settings->adc1command);
  
  //Signal no checking on the first ADC when compensating
  settings->checkfirstadc = 0;
  
  //Setup for reading the first ADC
  settings->compensation = settings->adc1compensation;
  settings->buffer = &settings->tracebuffer[1];
  
  //Read the data for the first ADC. Samples start on the second location, skipping every other sample
  fpga_read_adc_data(settings);
  
  //Save the calculated measurements
  settings->adc1rawaverage = settings->rawaverage;
  
  //Send command 0x1F to the FPGA followed by the translated data returned from command 0x14
  fpga_write_cmd(0x1F);
  fpga_write_short(triggerpoint);
  
  //Send a command for getting ADC2 trace data from the FPGA
  fpga_write_cmd(settings->adc2command);

  //Signal to check the readings of the first ADC on being zero when compensating
  settings->checkfirstadc = 1;

  //Setup for reading the first ADC
  settings->compensation = settings->adc2compensation;
  settings->buffer = &settings->tracebuffer[0];
  
  //Read the data for the second ADC. Samples start on the first location, skipping every other sample
  fpga_read_adc_data(settings);
  
  //Save the calculated measurements
  settings->adc2rawaverage = settings->rawaverage;
  
  //Calculate the overall average
  settings->average /= scopesettings.samplecount;

  //Calculate the peak to peak value
  settings->peakpeak = settings->max - settings->min;
  
  //Calculate the signal center
  settings->center = (settings->max + settings->min) / 2;
}

//----------------------------------------------------------------------------------------------------------------------------------

void fpga_read_adc_data(PCHANNELSETTINGS settings)
{
  register int32  sample;
  register uint32 count;
  register uint32 sum = 0;
  
  //Set the number of samples to read
  count = scopesettings.nofsamples;
  
  //Read the data as long as there is count
  while(count)
  {
    //Read the data
    sample = fpga_read_byte();
    
    //Sum the raw data for ADC difference calibration
    sum += sample;
    
    //Compensate the value for ADC in equality
    sample += settings->compensation;
    
    //Check if sample became negative
    if(sample < 0)
    {
      //Keep it on zero if so
      sample = 0;
    }
    
    //Check if sample over its max
    if(sample > 255)
    {
      //Keep it on max if so
      sample = 255;
    }
    
    //Check if busy with second ADC data
    if(settings->checkfirstadc)
    {
      //When ADC1 compensation is positive it bottoms out on this value
      if(settings->adc1compensation > 0)
      {
        //So when the compensated sample is below the positive ADC1 compensation value the reading need to be matched
        //The negative value is either equal or one less then the positive value in absolute sense
        if(sample < settings->adc1compensation)
        {
          //Match the two readings when within compensation range
          settings->buffer[1] = sample;
        }
      }
      else
      {
        //When ADC1 compensation is negative, ADC2 reading might need to be matched
        if(*settings->buffer < settings->adc2compensation)
        {
          //If that is the case get the adjusted value from the buffer
          sample = settings->buffer[1];
        }
      }
    }
    else
    {
      //When ADC2 compensation is positive it will bottom out on this value
      //So when the compensated sample is below the made positive ADC1 compensation value the readings need to be matched
      //For this the raw ADC1 sample is written to the ADC2 location
      
      //When ADC2 compensation is positive save the raw sample for possible matching
      if(settings->adc2compensation > 0)
      {
        settings->buffer[-1] = sample;
      }
    }
   
    //Get the minimum value of the samples
    if(sample < settings->min)
    {
      //Keep the lowest
      settings->min = sample;
    }
    
    //Get the maximum value of the samples
    if(sample > settings->max)
    {
      //Keep the highest
      settings->max = sample;
    }
    
    //Add the samples for average calculation
    settings->average += sample;
    
    
    //Store the data
    *settings->buffer = sample;
    
    //Skip a sample to allow for ADC2 data to be placed into
    settings->buffer += 2;
    
    //One read done
    count--;
  }
 
  //Calculate the raw average for ADC difference calibration
  settings->rawaverage = sum / scopesettings.nofsamples;
}

//----------------------------------------------------------------------------------------------------------------------------------

void fpga_set_battery_level(void)
{
}

//----------------------------------------------------------------------------------------------------------------------------------

void fpga_setup_for_calibration(void)
{
  //Set the channels to lowest sensitivity and trace offset
  scopesettings.channel1.voltperdiv  = 0;
//  scopesettings.channel1.traceoffset = 200;
  scopesettings.channel2.voltperdiv  = 0;
//  scopesettings.channel2.traceoffset = 200;
  
  //Set the time base to 100us/div
  scopesettings.timeperdiv = 17;
  
  //Load the settings into the FPGA
  fpga_set_channel_voltperdiv(&scopesettings.channel1);
  fpga_set_channel_offset(&scopesettings.channel1);

  fpga_set_channel_voltperdiv(&scopesettings.channel2);
  fpga_set_channel_offset(&scopesettings.channel1);
  
  fpga_set_sample_rate(scopesettings.samplerate);
  
  //Send the command for setting the trigger level to the FPGA
  fpga_write_cmd(0x17);
  
  //Write zero level to the FPGA
  fpga_write_byte(0);
  
  //Wait 100ms to settle
  timer0_delay(100);
  
  //Disable the trigger circuit??
  fpga_write_cmd(0x0F);
  fpga_write_byte(0x01);
}

//----------------------------------------------------------------------------------------------------------------------------------

void fpga_do_conversion(void)
{
  //Check if sampling with trigger system enabled
  if(scopesettings.samplemode == 1)
  {
    //Enable trigger system???
    fpga_write_cmd(0x0F);
    fpga_write_byte(0x00);
  }
  
  //Set the FPGA for short time base mode
  fpga_write_cmd(0x28);
  fpga_write_byte(0x00);
  
  //Reset the sample system
  fpga_write_cmd(0x01);
  fpga_write_byte(0x01);
  
  //Send check on ready command
  fpga_write_cmd(0x05);
  
  //Wait for the flag to become 1
  while((fpga_read_byte() & 1) == 0);
  
  //Test again to make sure it was no glitch?????
  while((fpga_read_byte() & 1) == 0);
  
  //Done with reset
  fpga_write_cmd(0x01);
  fpga_write_byte(0x00);
  
  //Send check on triggered or buffer full command to the FPGA
  fpga_write_cmd(0x0A);
  
  //Check if sampling with trigger system enabled
  if(scopesettings.samplemode == 1)
  {
    //Wait for the FPGA to signal triggered or touch panel is touched
    while(((fpga_read_byte() & 1) == 0) && (havetouch == 0))
    {
      //Scan the touch panel
      tp_i2c_read_status();
    }
    
    //Disable trigger system???
    fpga_write_cmd(0x0F);
    fpga_write_byte(0x01);
  }
  else
  {
    //Wait for the FPGA to signal triggered or buffer full
    while((fpga_read_byte() & 1) == 0);
  }
}

//----------------------------------------------------------------------------------------------------------------------------------

void fpga_set_channel_trace_offsets(uint32 offset)
{
  channel1_offset = (1500 - offset) * 0.170666666666667;
  channel2_offset = (1500 - offset) * 0.170666666666667;
}

//----------------------------------------------------------------------------------------------------------------------------------

void fpga_write_command_0x1F(uint32 data)
{
  fpga_write_cmd(0x1F);
  fpga_write_short(data);
}

//----------------------------------------------------------------------------------------------------------------------------------

uint32 fpga_get_center_level(uint32 command, uint32 voltperdiv)
{
  //Do a 1000 samples
  register uint32 count = 1000;
  register uint32 min = 0x0FFFFFFF;
  register uint32 max = 0x00000000;
  register uint32 sample;
  register uint32 signaladjust;
  
  uint32 channel;
  double scaler;
  double offset;
  
  //Translate this channel volts per div setting
  signaladjust = signal_adjusters[voltperdiv];
  
  //Freeze the signal generator to allow for proper sampling
  //Needed for the simulator only
  signalgeneratorfreeze();
  
  if(command == 0x24)
  {
    channel = 0;
    scaler = channel1_scaler;
    offset = channel1_offset;
  }
  else
  {
    channel = 1;
    scaler = channel2_scaler;
    offset = channel2_offset;
  }
  
  //Read the data as long as there is count
  while(count)
  {
    //Read the data
    sample = calculatesample(signalgeneratorgetsample(channel, (1001 - count), sample_rate_set), scaler, offset, 0);
  
    //Adjust the data for the correct voltage per div setting
    sample = (41954 * sample * signaladjust) >> 22;
    
    //Limit the sample on max displayable
    if(sample > 401)
    {
      sample = 401;
    }
    
    //Check against minimum value to get the lowest reading
    if(sample < min)
    {
      min = sample;
    }

    //Check against maximum value to get the highest reading
    if(sample > max)
    {
      max = sample;
    }
    
    //One sample done
    count --;
  }
  
  //Unfreeze the signal generator
  //Needed for the simulator only
  signalgeneratorunfreeze();
  
  //Return the center value based on the min and the max values
  return((max + min) / 2); 
}

//----------------------------------------------------------------------------------------------------------------------------------

void fpga_get_auto_set_values(uint32 flags)
{
  //Do a 1000 samples
  register uint32 count = 1000;
  register uint32 sample;
  
  //Freeze the signal generator to allow for proper sampling
  //Needed for the simulator only
  signalgeneratorfreeze();
  
  //Initialize the min and max when channel is selected
  if(flags & 0x01)
  {
    channel1_min = 0x0FFFFFFF;
    channel1_max = 0x00000000;
  }  

  //Initialize the min and max when channel is selected and enabled
  if(flags & 0x02)
  {
    channel2_min = 0x0FFFFFFF;
    channel2_max = 0x00000000;
  }  

  //Read the data as long as there is count
  while(count)
  {
    //Get and process the sample when the channels is selected
    if(flags & 0x01)
    {
      //Read the data
      sample = calculatesample(signalgeneratorgetsample(0, (1001 - count), sample_rate_set), channel1_scaler, channel1_offset, 0);

      //Adjust the data for the correct voltage per div setting
      sample = (41954 * sample * signal_adjusters[scopesettings.channel1.voltperdiv]) >> 22;

      //Limit the sample on max displayable
      if(sample > 401)
      {
        sample = 401;
      }

      //Check against minimum value to get the lowest reading
      if(sample < channel1_min)
      {
        channel1_min = sample;
      }

      //Check against maximum value to get the highest reading
      if(sample > channel1_max)
      {
        channel1_max = sample;
      }
    }
 
    //Get and process the sample when the channels is selected
    if(flags & 0x02)
    {
      //Read the data
      sample = calculatesample(signalgeneratorgetsample(1, (1001 - count), sample_rate_set), channel2_scaler, channel2_offset, 0);

      //Adjust the data for the correct voltage per div setting
      sample = (41954 * sample * signal_adjusters[scopesettings.channel2.voltperdiv]) >> 22;

      //Limit the sample on max displayable
      if(sample > 401)
      {
        sample = 401;
      }

      //Check against minimum value to get the lowest reading
      if(sample < channel2_min)
      {
        channel2_min = sample;
      }

      //Check against maximum value to get the highest reading
      if(sample > channel2_max)
      {
        channel2_max = sample;
      }
    }
    
    //One sample done
    count --;
  }
  
  //Calculate the center and vpp values when channel is selected
  if(flags & 0x01)
  {
    channel1_center = (channel1_max + channel1_min) >> 1;
    channel1_vpp    =  channel1_max - channel1_min;
  }  

  //Calculate the center and vpp values when channel is selected
  if(flags & 0x02)
  {
    channel2_center = (channel2_max + channel2_min) >> 1;
    channel2_vpp    =  channel2_max - channel2_min;
  }
  
  //Unfreeze the signal generator
  //Needed for the simulator only
  signalgeneratorunfreeze();
}

//----------------------------------------------------------------------------------------------------------------------------------

uint32 fpga_get_zero_crossings(uint32 channel)
{
  register uint32 count;
  register uint32 sample;
  register uint32 zerocrossings = 0;
  register uint32 state = 0;
  register uint32 highlevel;
  register uint32 lowlevel;
  register uint32 signaladjust;
  
  //Set the number of samples to process based on the time base setting
  if(scopesettings.timeperdiv < 4)
  {
    //For 50ms and 20ms it is only 750 samples
    count = 750;
  }
  else
  {
    //For 10ms - 10ns it is 1500 samples
    count = 1500;
  }
  
  //No idea what this is for, but in normal sample processing this is a value obtained from the special ic
  fpga_write_command_0x1F(100);

  if(channel == 0)
  {
    //Send the command for selecting the channels sample buffer
    fpga_write_cmd(0x20);
    
//    highlevel = scopesettings.channel1.traceoffset + 8;
//    lowlevel  = scopesettings.channel1.traceoffset - 8;
    highlevel = 136;
    lowlevel = 120;
    
    signaladjust = signal_adjusters[scopesettings.channel1.voltperdiv];
  }
  else
  {
    //Send the command for selecting the channels sample buffer
    fpga_write_cmd(0x22);
    
//    highlevel = scopesettings.channel2.traceoffset + 8;
//    lowlevel  = scopesettings.channel2.traceoffset - 8;
    highlevel = 136;
    lowlevel = 120;
    
    signaladjust = signal_adjusters[scopesettings.channel2.voltperdiv];
  }
  
  //Read the data as long as there is count
  while(count)
  {
    //Get the sample
    sample = fpga_read_byte();
    
    //Adjust the data for the correct voltage per div setting
    sample = (41954 * sample * signaladjust) >> 22;

    //Limit the sample on max displayable
    if(sample > 401)
    {
      sample = 401;
    }
    
    //Check against the high level
    if(sample > highlevel)
    {
      //If above check if in low state
      if(state == 0)
      {
        //If so flip the state
        state = 1;

        //Found a zero crossing
        zerocrossings++;
      }
    }
    //Check against the low level
    else if(sample < lowlevel)
    {
      //If below check if in high state
      if(state == 1)
      {
        //If so flip the state
        state = 0;
        
        //Found a zero crossing
        zerocrossings++;
      }
    }
    
    //One read done
    count--;
  }
  
  return(zerocrossings);
}

//----------------------------------------------------------------------------------------------------------------------------------

void sys_init_display(uint16 xsize, uint16 ysize, uint16 *address)
{
}

//----------------------------------------------------------------------------------------------------------------------------------

void tp_i2c_setup(void)
{
}

//----------------------------------------------------------------------------------------------------------------------------------

void tp_i2c_read_status(void)
{
  //Check if stop of process requested
  if(quit_scopeprocessing_thread_on_zero == 0)
  {
    //If so check if there was no touch
    if(havetouch == 0)
    {
      //Fake coordinates to force any menu to close and the process to stop
      havetouch = 1;

      //Check if the loop device is opened (USB connected)
      if(loop_device_open)
      {
        //Signal touch to turn it off
        xtouch = 120;
        ytouch = 250;
      }
      else
      {
        xtouch = 10;
        ytouch = 470;
      }
    }
    else
    {
      //Otherwise remove touch when stop is requested
      havetouch = 0;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------------------

void tp_i2c_wait_for_touch_release(void)
{
  //Wait until touch is released or process stopped
  while(havetouch && quit_scopeprocessing_thread_on_zero);
}

//----------------------------------------------------------------------------------------------------------------------------------

void usb_device_init(void)
{
}

//----------------------------------------------------------------------------------------------------------------------------------

void usb_device_disable(void)
{
  if(loop_device_open)
  {
    //Not the most elegant solution but it works more or less
    system("sudo umount /dev/loop0p1");
    system("sudo losetup -d /dev/loop0");
    
    loop_device_open = 0;
  }
}

//----------------------------------------------------------------------------------------------------------------------------------

void usb_device_enable(void)
{
  if(loop_device_open == 0)
  {
    //Not the most elegant solution but it works more or less
    system("sudo losetup /dev/loop0 scope_sd_card.img");
    system("sudo partprobe /dev/loop0");
    
    loop_device_open = 1;
  }
}

//----------------------------------------------------------------------------------------------------------------------------------

//Definitions of physical drive number for each drive
#define DEV_FILE     0 

DSTATUS disk_initialize(BYTE pdrv)
{
  //Check if the SD card device is addressed
  if(pdrv == DEV_FILE)
  {
    if(sd_emu_file == NULL)
    {
      //Try to initialize the interface and the card
      if((sd_emu_file = fopen("scope_sd_card.img", "rb+")) == NULL)
      {
        //Some error then signal no disk
        return(STA_NODISK);
      }
    }
  }
  else
  {
    //Not the SD card emulation drive then no init
    return(STA_NOINIT);
  }
  
  //All went well
  return(RES_OK);
}

//----------------------------------------------------------------------------------------------------------------------------------

void disk_exit(void)
{
  //Check if the sd card emulation file is open
  if(sd_emu_file)
  {
    //Close it if so
    fclose(sd_emu_file);
  }
}

//----------------------------------------------------------------------------------------------------------------------------------

DSTATUS disk_status(BYTE pdrv)
{
  //Check if the sd card emulation file is opened
  if(sd_emu_file == NULL)
  {
    //Not then signal no disk available
    return(STA_NODISK);
  }
  
  //Else everything ok
  return(RES_OK);
}

//----------------------------------------------------------------------------------------------------------------------------------

DRESULT disk_read(BYTE pdrv, BYTE *buff, LBA_t sector, UINT count)
{
  //Check if the SD card device is addressed
  if(pdrv == DEV_FILE)
  {
    //Check if the sd card emulation file is opened
    if(sd_emu_file == NULL)
    {
      //Not then signal no disk available
      return(STA_NODISK);
    }
    
    //Seek the requested location
    if(fseek(sd_emu_file, sector * 512, SEEK_SET))
    {
      //Error while seeking
      return(RES_ERROR);
    }
    
    //Read the data from the card emulation file
    fread(buff, 1, count * 512, sd_emu_file);
  }
  else
  {
    //Not the SD card drive selected
    return(RES_PARERR);
  }
  
  //All went well
  return(RES_OK);
}

//----------------------------------------------------------------------------------------------------------------------------------

DRESULT disk_write(BYTE pdrv, const BYTE *buff, LBA_t sector, UINT count)
{
  //Check if the SD card device is addressed
  if(pdrv == DEV_FILE)
  {
    //Check if the sd card emulation file is opened
    if(sd_emu_file == NULL)
    {
      //Not then signal no disk available
      return(STA_NODISK);
    }
    
    //Seek the requested location
    if(fseek(sd_emu_file, sector * 512, SEEK_SET))
    {
      //Error while seeking
      return(RES_ERROR);
    }
    
    //Write the data to the card emulation file
    fwrite(buff, 1, count * 512, sd_emu_file);
  }
  else
  {
    //Not the SD card drive selected
    return(RES_PARERR);
  }
  
  //All went well
  return(RES_OK);
}

//----------------------------------------------------------------------------------------------------------------------------------

DRESULT disk_ioctl(BYTE pdrv, BYTE cmd, void *buff)
{
  //Check if the SD card device is addressed
  if(pdrv == DEV_FILE)
  {
    //Check if the sd card emulation file is opened
    if(sd_emu_file == NULL)
    {
      //Not then signal no disk available
      return(STA_NODISK);
    }
    
    //Check if buffer is valid
    if(buff)
    {
      if(cmd == CTRL_SYNC)
      {
        return(RES_OK);
      }
      else if(cmd == GET_SECTOR_COUNT)
      {
        //Seek the requested location
        if(fseek(sd_emu_file, 0, SEEK_END))
        {
          //Error while seeking
          return(RES_ERROR);
        }
        
        *(uint32 *)buff = (ftell(sd_emu_file) + 511) / 512;
        
        return(RES_OK);
      }
    }
  }
  
  //Invalid input parameters given
  return(RES_PARERR);
}

//----------------------------------------------------------------------------------------------------------------------------------

int32 sd_card_read(uint32 sector, uint32 blocks, uint8 *buffer)
{
  //Check if the sd card emulation file is opened
  if(sd_emu_file == NULL)
  {
    //Not then signal card error
    return(-1);
  }

  //Seek the requested location
  if(fseek(sd_emu_file, sector * 512, SEEK_SET))
  {
    //Error while seeking
    return(-1);
  }

  //Read the data from the card emulation file
  fread(buffer, 1, blocks * 512, sd_emu_file);
  
  return(0);
}

//----------------------------------------------------------------------------------------------------------------------------------

int32 sd_card_write(uint32 sector, uint32 blocks, uint8 *buffer)
{
  //Check if the sd card emulation file is opened
  if(sd_emu_file == NULL)
  {
    //Not then signal card error
    return(-1);
  }

  //Seek the requested location
  if(fseek(sd_emu_file, sector * 512, SEEK_SET))
  {
    //Error while seeking
    return(-1);
  }

  //Write the data to the card emulation file
  fwrite(buffer, 1, blocks * 512, sd_emu_file);
  
  return(0);
}

//----------------------------------------------------------------------------------------------------------------------------------

DWORD get_fattime (void)
{
  //Some date and time value
  return(1349957149);
}

//----------------------------------------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------------------------------------
