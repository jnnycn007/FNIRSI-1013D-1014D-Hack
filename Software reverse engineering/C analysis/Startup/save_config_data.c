
void save_config_data(uint16 *address)

{
  byte *pbVar1;
  int iVar2;
  int iVar3;
  undefined2 *puVar4;
  ushort *puVar5;
  byte *pbVar6;
  int iVar7;
  
  puVar4 = DAT_80027d84;
  iVar3 = DAT_80027d80;
  iVar2 = DAT_80027d7c;
  pbVar1 = DAT_80027d78;
  *(ushort *)address = (ushort)*DAT_80027d78;
  puVar5 = (ushort *)((int)address + 0x14c);
  pbVar6 = (byte *)(iVar3 + -1);
  iVar7 = 0xc;
  *(ushort *)((int)address + 2) = (ushort)pbVar1[3];
  *(ushort *)((int)address + 4) = (ushort)pbVar1[4];
  *(ushort *)((int)address + 6) = (ushort)pbVar1[1];
  *(ushort *)((int)address + 8) = (ushort)pbVar1[2];
  *(ushort *)((int)address + 10) = (ushort)pbVar1[0xc];
  *(ushort *)((int)address + 0xc) = (ushort)pbVar1[0xf];
  *(ushort *)((int)address + 0xe) = (ushort)pbVar1[0x10];
  *(ushort *)(address + 1) = (ushort)pbVar1[0xd];
  *(ushort *)((int)address + 0x12) = (ushort)pbVar1[0xe];
  *(ushort *)((int)address + 0x14) = (ushort)pbVar1[10];
  *(ushort *)((int)address + 0x16) = (ushort)pbVar1[0x16];
  *(ushort *)((int)address + 0x18) = (ushort)pbVar1[0x21];
  *(ushort *)((int)address + 0x1a) = (ushort)pbVar1[0x22];
  *(ushort *)((int)address + 0x1c) = (ushort)pbVar1[0x23];
  *(ushort *)((int)address + 0x1e) = (ushort)pbVar1[0x42];
  *(undefined2 *)(address + 5) = *(undefined2 *)(pbVar1 + 0x24);
  *(undefined2 *)((int)address + 0x52) = *(undefined2 *)(pbVar1 + 0x26);
  *(undefined2 *)((int)address + 0x54) = *(undefined2 *)(pbVar1 + 6);
  *(undefined2 *)((int)address + 0x56) = *(undefined2 *)(pbVar1 + 0x12);
  *(ushort *)((int)address + 0x78) = (ushort)*(byte *)(iVar2 + 2);
  *(ushort *)((int)address + 0x7a) = (ushort)*(byte *)(iVar2 + 3);
  *(ushort *)((int)address + 0x7c) = (ushort)*(byte *)(iVar2 + 4);
  *(ushort *)((int)address + 0x7e) = (ushort)*(byte *)(iVar2 + 7);
  *(ushort *)(address + 10) = (ushort)*(byte *)(iVar3 + 0x100);
  *(ushort *)((int)address + 0xa2) = (ushort)*(byte *)(iVar3 + 0x112);
  *(ushort *)((int)address + 0xa4) = (ushort)*(byte *)(iVar3 + 0x122);
  *(ushort *)((int)address + 0xa6) = (ushort)*(byte *)(iVar3 + 0x132);
  *(ushort *)((int)address + 0xa8) = (ushort)*(byte *)(iVar3 + 0x142);
  *(ushort *)((int)address + 0xaa) = (ushort)*(byte *)(iVar3 + 0x152);
  *(ushort *)((int)address + 0xac) = (ushort)*(byte *)(iVar3 + 0x162);
  *(ushort *)((int)address + 0xae) = (ushort)*(byte *)(iVar3 + 0x172);
  *(ushort *)(address + 0xb) = (ushort)*(byte *)(iVar3 + 0x182);
  *(ushort *)((int)address + 0xb2) = (ushort)*(byte *)(iVar3 + 0x192);
  *(ushort *)((int)address + 0xb4) = (ushort)*(byte *)(iVar3 + 0x1a2);
  *(ushort *)((int)address + 0xb6) = (ushort)*(byte *)(iVar3 + 0x1b2);
  *(ushort *)((int)address + 0xb8) = (ushort)*(byte *)(iVar3 + 0x1c2);
  *(ushort *)((int)address + 0xba) = (ushort)*(byte *)(iVar3 + 0x1d2);
  *(ushort *)((int)address + 0xbc) = (ushort)*(byte *)(iVar3 + 0x1e2);
  *(ushort *)((int)address + 0xbe) = (ushort)*(byte *)(iVar3 + 0x1f2);
  *(ushort *)(address + 0xc) = (ushort)*(byte *)(iVar3 + 0x202);
  *(ushort *)((int)address + 0xc2) = (ushort)*(byte *)(iVar3 + 0x212);
  *(ushort *)((int)address + 0xc4) = (ushort)*(byte *)(iVar3 + 0x232);
  *(ushort *)((int)address + 0xc6) = (ushort)*(byte *)(iVar3 + 0x242);
  *(ushort *)((int)address + 200) = (ushort)*(byte *)(iVar3 + 0x252);
  *(ushort *)((int)address + 0xca) = (ushort)*(byte *)(iVar3 + 0x262);
  *(ushort *)((int)address + 0xcc) = (ushort)*(byte *)(iVar3 + 0x272);
  *(ushort *)((int)address + 0xce) = (ushort)*(byte *)(iVar3 + 0x282);
  *(undefined2 *)(address + 0xf) = *puVar4;
  *(undefined2 *)((int)address + 0xf2) = puVar4[1];
  *(undefined2 *)((int)address + 0xf4) = puVar4[2];
  *(undefined2 *)((int)address + 0xf6) = puVar4[3];
  *(undefined2 *)((int)address + 0xf8) = puVar4[4];
  *(undefined2 *)((int)address + 0xfa) = puVar4[5];
  *(undefined2 *)((int)address + 0xfc) = puVar4[6];
  *(undefined2 *)((int)address + 0xfe) = puVar4[7];
  *(undefined2 *)(address + 0x10) = puVar4[8];
  *(undefined2 *)((int)address + 0x102) = puVar4[9];
  *(undefined2 *)((int)address + 0x104) = puVar4[10];
  *(undefined2 *)((int)address + 0x106) = puVar4[0xb];
  *(undefined2 *)((int)address + 0x108) = puVar4[0xc];
  *(undefined2 *)((int)address + 0x10a) = puVar4[0xd];
  *(undefined2 *)((int)address + 0x10c) = puVar4[0xe];
  *(undefined2 *)((int)address + 0x10e) = puVar4[0xf];
  *(undefined2 *)(address + 0x11) = puVar4[0x10];
  *(undefined2 *)((int)address + 0x112) = puVar4[0x11];
  *(ushort *)((int)address + 0x114) = (ushort)*(byte *)(puVar4 + 0x12);
  *(ushort *)((int)address + 0x116) = (ushort)*(byte *)((int)puVar4 + 0x25);
  *(ushort *)((int)address + 0x118) = (ushort)*(byte *)(puVar4 + 0x13);
  *(ushort *)((int)address + 0x11a) = (ushort)*(byte *)((int)puVar4 + 0x27);
  *(ushort *)((int)address + 0x11c) = (ushort)*(byte *)(puVar4 + 0x14);
  *(ushort *)((int)address + 0x11e) = (ushort)*(byte *)((int)puVar4 + 0x29);
  *(undefined2 *)(address + 0x12) = puVar4[0x15];
  *(undefined2 *)((int)address + 0x122) = puVar4[0x16];
  *(undefined2 *)((int)address + 0x124) = puVar4[0x17];
  *(undefined2 *)((int)address + 0x126) = puVar4[0x18];
  *(undefined2 *)((int)address + 0x128) = puVar4[0x19];
  *(undefined2 *)((int)address + 0x12a) = puVar4[0x1a];
  *(ushort *)((int)address + 300) = (ushort)*(byte *)(puVar4 + 0x1b);
  *(ushort *)((int)address + 0x12e) = (ushort)*(byte *)((int)puVar4 + 0x37);
  *(ushort *)(address + 0x13) = (ushort)*(byte *)(puVar4 + 0x1c);
  *(ushort *)((int)address + 0x132) = (ushort)*(byte *)((int)puVar4 + 0x39);
  *(ushort *)((int)address + 0x134) = (ushort)*(byte *)(puVar4 + 0x1d);
  *(ushort *)((int)address + 0x136) = (ushort)*(byte *)((int)puVar4 + 0x3b);
  *(undefined2 *)((int)address + 0x138) = puVar4[0x1e];
  *(undefined2 *)((int)address + 0x13a) = puVar4[0x1f];
  *(undefined2 *)((int)address + 0x13c) = puVar4[0x20];
  *(undefined2 *)((int)address + 0x13e) = puVar4[0x21];
  *(undefined2 *)(address + 0x14) = puVar4[0x22];
  *(undefined2 *)((int)address + 0x142) = puVar4[0x23];
  *(ushort *)((int)address + 0x142) = (ushort)*(byte *)(iVar3 + 0x292);
  *(undefined2 *)((int)address + 0x144) = *(undefined2 *)(iVar3 + 0x294);
  *(undefined2 *)((int)address + 0x146) = *(undefined2 *)(iVar3 + 0x296);
  *(ushort *)((int)address + 0x148) = (ushort)*(byte *)(iVar3 + 0x29a);
  *(undefined2 *)((int)address + 0x14a) = *(undefined2 *)(iVar3 + 0x29c);
  *(undefined2 *)((int)address + 0x14c) = *(undefined2 *)(iVar3 + 0x29e);

  do {
    iVar7 = iVar7 + -1;
    puVar5[1] = (ushort)pbVar6[1];
    pbVar6 = pbVar6 + 2;
    puVar5 = puVar5 + 2;
    *puVar5 = (ushort)*pbVar6;
  } while (iVar7 != 0);

  *(undefined2 *)(address + 0x19) = *(undefined2 *)(pbVar1 + 0x44);
  return;
}


