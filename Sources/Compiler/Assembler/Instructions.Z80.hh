Z80_OPCODE_2(ADC, A, A,             ({ 0x8F                                         }),  4);
Z80_OPCODE_2(ADC, A, B,             ({ 0x88                                         }),  4);
Z80_OPCODE_2(ADC, A, C,             ({ 0x89                                         }),  4);
Z80_OPCODE_2(ADC, A, D,             ({ 0x8A                                         }),  4);
Z80_OPCODE_2(ADC, A, E,             ({ 0x8B                                         }),  4);
Z80_OPCODE_2(ADC, A, H,             ({ 0x8C                                         }),  4);
Z80_OPCODE_2(ADC, A, L,             ({ 0x8D                                         }),  4);
Z80_OPCODE_2(ADC, A, byte,          ({ 0xCE, OP2                                    }),  7);
Z80_OPCODE_2(ADC, A, memHL,         ({ 0x8E                                         }),  7);
Z80_OPCODE_2(ADC, A, IXH,           ({ 0xDD, 0x8C                                   }),  8);    // undocumented
Z80_OPCODE_2(ADC, A, IYH,           ({ 0xFD, 0x8C                                   }),  8);    // undocumented
Z80_OPCODE_2(ADC, A, IXL,           ({ 0xDD, 0x8D                                   }),  8);    // undocumented
Z80_OPCODE_2(ADC, A, IYL,           ({ 0xFD, 0x8D                                   }),  8);    // undocumented
Z80_OPCODE_2(ADC, A, IX_byte,       ({ 0xDD, 0x8E, OP2                              }), 19);
Z80_OPCODE_2(ADC, A, IY_byte,       ({ 0xFD, 0x8E, OP2                              }), 19);
Z80_OPCODE_2(ADC, HL, BC,           ({ 0xED, 0x4A                                   }), 15);
Z80_OPCODE_2(ADC, HL, DE,           ({ 0xED, 0x5A                                   }), 15);
Z80_OPCODE_2(ADC, HL, HL,           ({ 0xED, 0x6A                                   }), 15);
Z80_OPCODE_2(ADC, HL, SP,           ({ 0xED, 0x7A                                   }), 15);
Z80_OPCODE_2(ADD, A, A,             ({ 0x87                                         }),  4);
Z80_OPCODE_2(ADD, A, B,             ({ 0x80                                         }),  4);
Z80_OPCODE_2(ADD, A, C,             ({ 0x81                                         }),  4);
Z80_OPCODE_2(ADD, A, D,             ({ 0x82                                         }),  4);
Z80_OPCODE_2(ADD, A, E,             ({ 0x83                                         }),  4);
Z80_OPCODE_2(ADD, A, H,             ({ 0x84                                         }),  4);
Z80_OPCODE_2(ADD, A, L,             ({ 0x85                                         }),  4);
Z80_OPCODE_2(ADD, A, byte,          ({ 0xC6, OP2                                    }),  7);
Z80_OPCODE_2(ADD, A, memHL,         ({ 0x86                                         }),  7);
Z80_OPCODE_2(ADD, A, IXH,           ({ 0xDD, 0x84                                   }),  8);    // undocumented
Z80_OPCODE_2(ADD, A, IYH,           ({ 0xFD, 0x84                                   }),  8);    // undocumented
Z80_OPCODE_2(ADD, A, IXL,           ({ 0xDD, 0x85                                   }),  8);    // undocumented
Z80_OPCODE_2(ADD, A, IYL,           ({ 0xFD, 0x85                                   }),  8);    // undocumented
Z80_OPCODE_2(ADD, A, IX_byte,       ({ 0xDD, 0x86, OP2                              }), 19);
Z80_OPCODE_2(ADD, A, IY_byte,       ({ 0xFD, 0x86, OP2                              }), 19);
Z80_OPCODE_2(ADD, HL, BC,           ({ 0x09                                         }), 11);
Z80_OPCODE_2(ADD, HL, DE,           ({ 0x19                                         }), 11);
Z80_OPCODE_2(ADD, HL, HL,           ({ 0x29                                         }), 11);
Z80_OPCODE_2(ADD, HL, SP,           ({ 0x39                                         }), 11);
Z80_OPCODE_2(ADD, IX, BC,           ({ 0xDD, 0x09                                   }), 15);
Z80_OPCODE_2(ADD, IX, DE,           ({ 0xDD, 0x19                                   }), 15);
Z80_OPCODE_2(ADD, IX, IX,           ({ 0xDD, 0x29                                   }), 15);
Z80_OPCODE_2(ADD, IX, SP,           ({ 0xDD, 0x39                                   }), 15);
Z80_OPCODE_2(ADD, IY, BC,           ({ 0xFD, 0x09                                   }), 15);
Z80_OPCODE_2(ADD, IY, DE,           ({ 0xFD, 0x19                                   }), 15);
Z80_OPCODE_2(ADD, IY, IY,           ({ 0xFD, 0x29                                   }), 15);
Z80_OPCODE_2(ADD, IY, SP,           ({ 0xFD, 0x39                                   }), 15);
Z80_OPCODE_1(AND, A,                ({ 0xA7                                         }),  4);
Z80_OPCODE_1(AND, B,                ({ 0xA0                                         }),  4);
Z80_OPCODE_1(AND, C,                ({ 0xA1                                         }),  4);
Z80_OPCODE_1(AND, D,                ({ 0xA2                                         }),  4);
Z80_OPCODE_1(AND, E,                ({ 0xA3                                         }),  4);
Z80_OPCODE_1(AND, H,                ({ 0xA4                                         }),  4);
Z80_OPCODE_1(AND, L,                ({ 0xA5                                         }),  4);
Z80_OPCODE_1(AND, byte,             ({ 0xE6, OP1                                    }),  7);
Z80_OPCODE_1(AND, memHL,            ({ 0xA6                                         }),  7);
Z80_OPCODE_1(AND, IX_byte,          ({ 0xDD, 0xA6, OP1                              }), 19);
Z80_OPCODE_1(AND, IY_byte,          ({ 0xFD, 0xA6, OP1                              }), 19);
Z80_OPCODE_1(AND, IXH,              ({ 0xDD, 0xA4                                   }),  8);    // undocumented
Z80_OPCODE_1(AND, IYH,              ({ 0xFD, 0xA4                                   }),  8);    // undocumented
Z80_OPCODE_1(AND, IXL,              ({ 0xDD, 0xA5                                   }),  8);    // undocumented
Z80_OPCODE_1(AND, IYL,              ({ 0xFD, 0xA5                                   }),  8);    // undocumented
Z80_OPCODE_2(BIT, bit, memHL,       ({ 0xCB, OP1V(0x46)                             }), 12);
Z80_OPCODE_2(BIT, bit, IX_byte,     ({ 0xDD, 0xCB, OP2, OP1V(0x46)                  }), 20);
Z80_OPCODE_2(BIT, bit, IY_byte,     ({ 0xFD, 0xCB, OP2, OP1V(0x46)                  }), 20);
Z80_OPCODE_2(BIT, bit, A,           ({ 0xCB, OP1V(0x47)                             }),  8);
Z80_OPCODE_2(BIT, bit, B,           ({ 0xCB, OP1V(0x40)                             }),  8);
Z80_OPCODE_2(BIT, bit, C,           ({ 0xCB, OP1V(0x41)                             }),  8);
Z80_OPCODE_2(BIT, bit, D,           ({ 0xCB, OP1V(0x42)                             }),  8);
Z80_OPCODE_2(BIT, bit, E,           ({ 0xCB, OP1V(0x43)                             }),  8);
Z80_OPCODE_2(BIT, bit, H,           ({ 0xCB, OP1V(0x44)                             }),  8);
Z80_OPCODE_2(BIT, bit, L,           ({ 0xCB, OP1V(0x45)                             }),  8);
Z80_OPCODE_1(CALL, word,            ({ 0xCD, OP1W                                   }), 17);
Z80_OPCODE_2(CALL, flagC, word,     ({ 0xDC, OP2W                                   }), TStates(17, 10));
Z80_OPCODE_2(CALL, flagM, word,     ({ 0xFC, OP2W                                   }), TStates(17, 10));
Z80_OPCODE_2(CALL, flagNC, word,    ({ 0xD4, OP2W                                   }), TStates(17, 10));
Z80_OPCODE_2(CALL, flagNZ, word,    ({ 0xC4, OP2W                                   }), TStates(17, 10));
Z80_OPCODE_2(CALL, flagP, word,     ({ 0xF4, OP2W                                   }), TStates(17, 10));
Z80_OPCODE_2(CALL, flagPE, word,    ({ 0xEC, OP2W                                   }), TStates(17, 10));
Z80_OPCODE_2(CALL, flagPO, word,    ({ 0xE4, OP2W                                   }), TStates(17, 10));
Z80_OPCODE_2(CALL, flagZ, word,     ({ 0xCC, OP2W                                   }), TStates(17, 10));
Z80_OPCODE_0(CCF,                   ({ 0x3F                                         }),  4);
Z80_OPCODE_1(CP, A,                 ({ 0xBF                                         }),  4);
Z80_OPCODE_1(CP, B,                 ({ 0xB8                                         }),  4);
Z80_OPCODE_1(CP, C,                 ({ 0xB9                                         }),  4);
Z80_OPCODE_1(CP, D,                 ({ 0xBA                                         }),  4);
Z80_OPCODE_1(CP, E,                 ({ 0xBB                                         }),  4);
Z80_OPCODE_1(CP, H,                 ({ 0xBC                                         }),  4);
Z80_OPCODE_1(CP, L,                 ({ 0xBD                                         }),  4);
Z80_OPCODE_1(CP, byte,              ({ 0xFE, OP1                                    }),  7);
Z80_OPCODE_1(CP, memHL,             ({ 0xBE                                         }),  7);
Z80_OPCODE_1(CP, IX_byte,           ({ 0xDD, 0xBE, OP1                              }), 19);
Z80_OPCODE_1(CP, IY_byte,           ({ 0xFD, 0xBE, OP1                              }), 19);
Z80_OPCODE_1(CP, IXH,               ({ 0xDD, 0xBC                                   }),  8);    // undocumented
Z80_OPCODE_1(CP, IYH,               ({ 0xFD, 0xBC                                   }),  8);    // undocumented
Z80_OPCODE_1(CP, IXL,               ({ 0xDD, 0xBD                                   }),  8);    // undocumented
Z80_OPCODE_1(CP, IYL,               ({ 0xFD, 0xBD                                   }),  8);    // undocumented
Z80_OPCODE_0(CPD,                   ({ 0xED, 0xA9                                   }), 16);
Z80_OPCODE_0(CPDR,                  ({ 0xED, 0xB9                                   }), TStates(21, 16));
Z80_OPCODE_0(CPI,                   ({ 0xED, 0xA1                                   }), 16);
Z80_OPCODE_0(CPIR,                  ({ 0xED, 0xB1                                   }), TStates(21, 16));
Z80_OPCODE_0(CPL,                   ({ 0x2F                                         }),  4);
Z80_OPCODE_0(DAA,                   ({ 0x27                                         }),  4);
Z80_OPCODE_1(DEC, A,                ({ 0x3D                                         }),  4);
Z80_OPCODE_1(DEC, B,                ({ 0x05                                         }),  4);
Z80_OPCODE_1(DEC, C,                ({ 0x0D                                         }),  4);
Z80_OPCODE_1(DEC, D,                ({ 0x15                                         }),  4);
Z80_OPCODE_1(DEC, E,                ({ 0x1D                                         }),  4);
Z80_OPCODE_1(DEC, H,                ({ 0x25                                         }),  4);
Z80_OPCODE_1(DEC, L,                ({ 0x2D                                         }),  4);
Z80_OPCODE_1(DEC, memHL,            ({ 0x35                                         }), 11);
Z80_OPCODE_1(DEC, IX_byte,          ({ 0xDD, 0x35, OP1                              }), 23);
Z80_OPCODE_1(DEC, IY_byte,          ({ 0xFD, 0x35, OP1                              }), 23);
Z80_OPCODE_1(DEC, IX,               ({ 0xDD, 0x2B                                   }), 10);
Z80_OPCODE_1(DEC, IY,               ({ 0xFD, 0x2B                                   }), 10);
Z80_OPCODE_1(DEC, BC,               ({ 0x0B                                         }),  6);
Z80_OPCODE_1(DEC, DE,               ({ 0x1B                                         }),  6);
Z80_OPCODE_1(DEC, HL,               ({ 0x2B                                         }),  6);
Z80_OPCODE_1(DEC, SP,               ({ 0x3B                                         }),  6);
Z80_OPCODE_1(DEC, IXH,              ({ 0xDD, 0x25                                   }),  8);    // undocumented
Z80_OPCODE_1(DEC, IYH,              ({ 0xFD, 0x25                                   }),  8);    // undocumented
Z80_OPCODE_1(DEC, IXL,              ({ 0xDD, 0x2D                                   }),  8);    // undocumented
Z80_OPCODE_1(DEC, IYL,              ({ 0xFD, 0x2D                                   }),  8);    // undocumented
Z80_OPCODE_0(DI,                    ({ 0xF3                                         }),  4);
Z80_OPCODE_1(DJNZ, relOffset,       ({ 0x10, OP1V(NEXT)                             }), TStates(8, 13));
Z80_OPCODE_0(EI,                    ({ 0xFB                                         }),  4);
Z80_OPCODE_2(EX, memSP, HL,         ({ 0xE3                                         }), 19);
Z80_OPCODE_2(EX, memSP, IX,         ({ 0xDD, 0xE3                                   }), 23);
Z80_OPCODE_2(EX, memSP, IY,         ({ 0xFD, 0xE3                                   }), 23);
Z80_OPCODE_2(EX, AF, AF_,           ({ 0x08                                         }),  4);
Z80_OPCODE_2(EX, DE, HL,            ({ 0xEB                                         }),  4);
Z80_OPCODE_0(EXX,                   ({ 0xD9                                         }),  4);
Z80_OPCODE_0(HALT,                  ({ 0x76                                         }),  4);
Z80_OPCODE_1(IM, intMode,           ({ 0xED, OP1                                    }),  8);
Z80_OPCODE_2(IN, A, portAddr,       ({ 0xDB, OP2                                    }), 11);
Z80_OPCODE_2(IN, A, portC,          ({ 0xED, 0x78                                   }), 12);
Z80_OPCODE_2(IN, B, portC,          ({ 0xED, 0x40                                   }), 12);
Z80_OPCODE_2(IN, C, portC,          ({ 0xED, 0x48                                   }), 12);
Z80_OPCODE_2(IN, D, portC,          ({ 0xED, 0x50                                   }), 12);
Z80_OPCODE_2(IN, E, portC,          ({ 0xED, 0x58                                   }), 12);
Z80_OPCODE_2(IN, H, portC,          ({ 0xED, 0x60                                   }), 12);
Z80_OPCODE_2(IN, L, portC,          ({ 0xED, 0x68                                   }), 12);
Z80_OPCODE_1(INC, A,                ({ 0x3C                                         }),  4);
Z80_OPCODE_1(INC, B,                ({ 0x04                                         }),  4);
Z80_OPCODE_1(INC, C,                ({ 0x0C                                         }),  4);
Z80_OPCODE_1(INC, D,                ({ 0x14                                         }),  4);
Z80_OPCODE_1(INC, E,                ({ 0x1C                                         }),  4);
Z80_OPCODE_1(INC, H,                ({ 0x24                                         }),  4);
Z80_OPCODE_1(INC, L,                ({ 0x2C                                         }),  4);
Z80_OPCODE_1(INC, memHL,            ({ 0x34                                         }), 11);
Z80_OPCODE_1(INC, IX_byte,          ({ 0xDD, 0x34, OP1                              }), 23);
Z80_OPCODE_1(INC, IY_byte,          ({ 0xFD, 0x34, OP1                              }), 23);
Z80_OPCODE_1(INC, IX,               ({ 0xDD, 0x23                                   }), 10);
Z80_OPCODE_1(INC, IY,               ({ 0xFD, 0x23                                   }), 10);
Z80_OPCODE_1(INC, BC,               ({ 0x03                                         }),  6);
Z80_OPCODE_1(INC, DE,               ({ 0x13                                         }),  6);
Z80_OPCODE_1(INC, HL,               ({ 0x23                                         }),  6);
Z80_OPCODE_1(INC, SP,               ({ 0x33                                         }),  6);
Z80_OPCODE_1(INC, IXH,              ({ 0xDD, 0x24                                   }),  8);    // undocumented
Z80_OPCODE_1(INC, IYH,              ({ 0xFD, 0x24                                   }),  8);    // undocumented
Z80_OPCODE_1(INC, IXL,              ({ 0xDD, 0x2C                                   }),  8);    // undocumented
Z80_OPCODE_1(INC, IYL,              ({ 0xFD, 0x2C                                   }),  8);    // undocumented
Z80_OPCODE_0(IND,                   ({ 0xED, 0xAA                                   }), 16);
Z80_OPCODE_0(INDR,                  ({ 0xED, 0xBA                                   }), TStates(21, 16));
Z80_OPCODE_0(INI,                   ({ 0xED, 0xA2                                   }), 16);
Z80_OPCODE_0(INIR,                  ({ 0xED, 0xB2                                   }), TStates(21, 16));
Z80_OPCODE_1(JP, word,              ({ 0xC3, OP1W                                   }), 10);
Z80_OPCODE_2(JP, flagC, word,       ({ 0xDA, OP2W                                   }), 10);
Z80_OPCODE_2(JP, flagM, word,       ({ 0xFA, OP2W                                   }), 10);
Z80_OPCODE_2(JP, flagNC, word,      ({ 0xD2, OP2W                                   }), 10);
Z80_OPCODE_2(JP, flagNZ, word,      ({ 0xC2, OP2W                                   }), 10);
Z80_OPCODE_2(JP, flagP, word,       ({ 0xF2, OP2W                                   }), 10);
Z80_OPCODE_2(JP, flagPE, word,      ({ 0xEA, OP2W                                   }), 10);
Z80_OPCODE_2(JP, flagPO, word,      ({ 0xE2, OP2W                                   }), 10);
Z80_OPCODE_2(JP, flagZ, word,       ({ 0xCA, OP2W                                   }), 10);
Z80_OPCODE_1(JP, memHL,             ({ 0xE9                                         }),  4);
Z80_OPCODE_1(JP, memIX,             ({ 0xDD, 0xE9                                   }),  8);
Z80_OPCODE_1(JP, memIY,             ({ 0xFD, 0xE9                                   }),  8);
Z80_OPCODE_1(JR, relOffset,         ({ 0x18, OP1V(NEXT)                             }),  12);
Z80_OPCODE_2(JR, flagC, relOffset,  ({ 0x38, OP2V(NEXT)                             }), TStates(12, 7));
Z80_OPCODE_2(JR, flagNC, relOffset, ({ 0x30, OP2V(NEXT)                             }), TStates(12, 7));
Z80_OPCODE_2(JR, flagNZ, relOffset, ({ 0x20, OP2V(NEXT)                             }), TStates(12, 7));
Z80_OPCODE_2(JR, flagZ, relOffset,  ({ 0x28, OP2V(NEXT)                             }), TStates(12, 7));
Z80_OPCODE_2(LD, A, memAddr,        ({ 0x3A, OP2W                                   }), 13);
Z80_OPCODE_2(LD, A, memBC,          ({ 0x0A                                         }),  7);
Z80_OPCODE_2(LD, A, memDE,          ({ 0x1A                                         }),  7);
Z80_OPCODE_2(LD, A, I,              ({ 0xED, 0x57                                   }),  9);
Z80_OPCODE_2(LD, A, R,              ({ 0xED, 0x5F                                   }),  9);
Z80_OPCODE_2(LD, memBC, A,          ({ 0x02                                         }),  7);
Z80_OPCODE_2(LD, memDE, A,          ({ 0x12                                         }),  7);
Z80_OPCODE_2(LD, memHL, byte,       ({ 0x36, OP2                                    }), 10);
Z80_OPCODE_2(LD, IX_byte, byte,     ({ 0xDD, 0x36, OP1, OP2                         }), 19);
Z80_OPCODE_2(LD, IY_byte, byte,     ({ 0xFD, 0x36, OP1, OP2                         }), 19);
Z80_OPCODE_2(LD, memAddr, A,        ({ 0x32, OP1W                                   }), 13);
Z80_OPCODE_2(LD, memAddr, HL,       ({ 0x22, OP1W                                   }), 16);
Z80_OPCODE_2(LD, memAddr, BC,       ({ 0xED, 0x43, OP1W                             }), 20);
Z80_OPCODE_2(LD, memAddr, DE,       ({ 0xED, 0x53, OP1W                             }), 20);
Z80_OPCODE_2(LD, memAddr, SP,       ({ 0xED, 0x73, OP1W                             }), 20);
Z80_OPCODE_2(LD, memAddr, IX,       ({ 0xDD, 0x22, OP1W                             }), 20);
Z80_OPCODE_2(LD, memAddr, IY,       ({ 0xFD, 0x22, OP1W                             }), 20);
Z80_OPCODE_2(LD, BC, word,          ({ 0x01, OP2W                                   }), 10);
Z80_OPCODE_2(LD, DE, word,          ({ 0x11, OP2W                                   }), 10);
Z80_OPCODE_2(LD, HL, word,          ({ 0x21, OP2W                                   }), 10);
Z80_OPCODE_2(LD, SP, word,          ({ 0x31, OP2W                                   }), 10);
Z80_OPCODE_2(LD, BC, memAddr,       ({ 0xED, 0x4B, OP2W                             }), 20);
Z80_OPCODE_2(LD, DE, memAddr,       ({ 0xED, 0x5B, OP2W                             }), 20);
Z80_OPCODE_2(LD, HL, memAddr,       ({ 0x2A, OP2W                                   }), 16);
Z80_OPCODE_2(LD, SP, memAddr,       ({ 0xED, 0x7B, OP2W                             }), 20);
Z80_OPCODE_2(LD, memHL, A,          ({ 0x77                                         }),  7);
Z80_OPCODE_2(LD, memHL, B,          ({ 0x70                                         }),  7);
Z80_OPCODE_2(LD, memHL, C,          ({ 0x71                                         }),  7);
Z80_OPCODE_2(LD, memHL, D,          ({ 0x72                                         }),  7);
Z80_OPCODE_2(LD, memHL, E,          ({ 0x73                                         }),  7);
Z80_OPCODE_2(LD, memHL, H,          ({ 0x74                                         }),  7);
Z80_OPCODE_2(LD, memHL, L,          ({ 0x75                                         }),  7);
Z80_OPCODE_2(LD, IX_byte, A,        ({ 0xDD, 0x77, OP1                              }), 19);
Z80_OPCODE_2(LD, IX_byte, B,        ({ 0xDD, 0x70, OP1                              }), 19);
Z80_OPCODE_2(LD, IX_byte, C,        ({ 0xDD, 0x71, OP1                              }), 19);
Z80_OPCODE_2(LD, IX_byte, D,        ({ 0xDD, 0x72, OP1                              }), 19);
Z80_OPCODE_2(LD, IX_byte, E,        ({ 0xDD, 0x73, OP1                              }), 19);
Z80_OPCODE_2(LD, IX_byte, H,        ({ 0xDD, 0x74, OP1                              }), 19);
Z80_OPCODE_2(LD, IX_byte, L,        ({ 0xDD, 0x75, OP1                              }), 19);
Z80_OPCODE_2(LD, IY_byte, A,        ({ 0xFD, 0x77, OP1                              }), 19);
Z80_OPCODE_2(LD, IY_byte, B,        ({ 0xFD, 0x70, OP1                              }), 19);
Z80_OPCODE_2(LD, IY_byte, C,        ({ 0xFD, 0x71, OP1                              }), 19);
Z80_OPCODE_2(LD, IY_byte, D,        ({ 0xFD, 0x72, OP1                              }), 19);
Z80_OPCODE_2(LD, IY_byte, E,        ({ 0xFD, 0x73, OP1                              }), 19);
Z80_OPCODE_2(LD, IY_byte, H,        ({ 0xFD, 0x74, OP1                              }), 19);
Z80_OPCODE_2(LD, IY_byte, L,        ({ 0xFD, 0x75, OP1                              }), 19);
Z80_OPCODE_2(LD, I, A,              ({ 0xED, 0x47                                   }),  9);
Z80_OPCODE_2(LD, IX, word,          ({ 0xDD, 0x21, OP2W                             }), 14);
Z80_OPCODE_2(LD, IX, memAddr,       ({ 0xDD, 0x2A, OP2W                             }), 20);
Z80_OPCODE_2(LD, IY, word,          ({ 0xFD, 0x21, OP2W                             }), 14);
Z80_OPCODE_2(LD, IY, memAddr,       ({ 0xFD, 0x2A, OP2W                             }), 20);
Z80_OPCODE_2(LD, R, A,              ({ 0xED, 0x4F                                   }),  9);
Z80_OPCODE_2(LD, A, memHL,          ({ 0x7E                                         }),  7);
Z80_OPCODE_2(LD, B, memHL,          ({ 0x46                                         }),  7);
Z80_OPCODE_2(LD, C, memHL,          ({ 0x4E                                         }),  7);
Z80_OPCODE_2(LD, D, memHL,          ({ 0x56                                         }),  7);
Z80_OPCODE_2(LD, E, memHL,          ({ 0x5E                                         }),  7);
Z80_OPCODE_2(LD, H, memHL,          ({ 0x66                                         }),  7);
Z80_OPCODE_2(LD, L, memHL,          ({ 0x6E                                         }),  7);
Z80_OPCODE_2(LD, A, IX_byte,        ({ 0xDD, 0x7E, OP2                              }), 19);
Z80_OPCODE_2(LD, B, IX_byte,        ({ 0xDD, 0x46, OP2                              }), 19);
Z80_OPCODE_2(LD, C, IX_byte,        ({ 0xDD, 0x4E, OP2                              }), 19);
Z80_OPCODE_2(LD, D, IX_byte,        ({ 0xDD, 0x56, OP2                              }), 19);
Z80_OPCODE_2(LD, E, IX_byte,        ({ 0xDD, 0x5E, OP2                              }), 19);
Z80_OPCODE_2(LD, H, IX_byte,        ({ 0xDD, 0x66, OP2                              }), 19);
Z80_OPCODE_2(LD, L, IX_byte,        ({ 0xDD, 0x6E, OP2                              }), 19);
Z80_OPCODE_2(LD, A, IY_byte,        ({ 0xFD, 0x7E, OP2                              }), 19);
Z80_OPCODE_2(LD, B, IY_byte,        ({ 0xFD, 0x46, OP2                              }), 19);
Z80_OPCODE_2(LD, C, IY_byte,        ({ 0xFD, 0x4E, OP2                              }), 19);
Z80_OPCODE_2(LD, D, IY_byte,        ({ 0xFD, 0x56, OP2                              }), 19);
Z80_OPCODE_2(LD, E, IY_byte,        ({ 0xFD, 0x5E, OP2                              }), 19);
Z80_OPCODE_2(LD, H, IY_byte,        ({ 0xFD, 0x66, OP2                              }), 19);
Z80_OPCODE_2(LD, L, IY_byte,        ({ 0xFD, 0x6E, OP2                              }), 19);
Z80_OPCODE_2(LD, A, byte,           ({ 0x3E, OP2                                    }),  7);
Z80_OPCODE_2(LD, B, byte,           ({ 0x06, OP2                                    }),  7);
Z80_OPCODE_2(LD, C, byte,           ({ 0x0E, OP2                                    }),  7);
Z80_OPCODE_2(LD, D, byte,           ({ 0x16, OP2                                    }),  7);
Z80_OPCODE_2(LD, E, byte,           ({ 0x1E, OP2                                    }),  7);
Z80_OPCODE_2(LD, H, byte,           ({ 0x26, OP2                                    }),  7);
Z80_OPCODE_2(LD, L, byte,           ({ 0x2E, OP2                                    }),  7);
Z80_OPCODE_2(LD, IXH, byte,         ({ 0xDD, 0x26, OP2                              }), 11);    // undocumented
Z80_OPCODE_2(LD, IYH, byte,         ({ 0xFD, 0x26, OP2                              }), 11);    // undocumented
Z80_OPCODE_2(LD, IXL, byte,         ({ 0xDD, 0x2E, OP2                              }), 11);    // undocumented
Z80_OPCODE_2(LD, IYL, byte,         ({ 0xFD, 0x2E, OP2                              }), 11);    // undocumented
Z80_OPCODE_2(LD, A, A,              ({ 0x7F                                         }),  4);
Z80_OPCODE_2(LD, B, A,              ({ 0x47                                         }),  4);
Z80_OPCODE_2(LD, C, A,              ({ 0x4F                                         }),  4);
Z80_OPCODE_2(LD, D, A,              ({ 0x57                                         }),  4);
Z80_OPCODE_2(LD, E, A,              ({ 0x5F                                         }),  4);
Z80_OPCODE_2(LD, H, A,              ({ 0x67                                         }),  4);
Z80_OPCODE_2(LD, L, A,              ({ 0x6F                                         }),  4);
Z80_OPCODE_2(LD, A, B,              ({ 0x78                                         }),  4);
Z80_OPCODE_2(LD, B, B,              ({ 0x40                                         }),  4);
Z80_OPCODE_2(LD, C, B,              ({ 0x48                                         }),  4);
Z80_OPCODE_2(LD, D, B,              ({ 0x50                                         }),  4);
Z80_OPCODE_2(LD, E, B,              ({ 0x58                                         }),  4);
Z80_OPCODE_2(LD, H, B,              ({ 0x60                                         }),  4);
Z80_OPCODE_2(LD, L, B,              ({ 0x68                                         }),  4);
Z80_OPCODE_2(LD, A, C,              ({ 0x79                                         }),  4);
Z80_OPCODE_2(LD, B, C,              ({ 0x41                                         }),  4);
Z80_OPCODE_2(LD, C, C,              ({ 0x49                                         }),  4);
Z80_OPCODE_2(LD, D, C,              ({ 0x51                                         }),  4);
Z80_OPCODE_2(LD, E, C,              ({ 0x59                                         }),  4);
Z80_OPCODE_2(LD, H, C,              ({ 0x61                                         }),  4);
Z80_OPCODE_2(LD, L, C,              ({ 0x69                                         }),  4);
Z80_OPCODE_2(LD, A, D,              ({ 0x7A                                         }),  4);
Z80_OPCODE_2(LD, B, D,              ({ 0x42                                         }),  4);
Z80_OPCODE_2(LD, C, D,              ({ 0x4A                                         }),  4);
Z80_OPCODE_2(LD, D, D,              ({ 0x52                                         }),  4);
Z80_OPCODE_2(LD, E, D,              ({ 0x5A                                         }),  4);
Z80_OPCODE_2(LD, H, D,              ({ 0x62                                         }),  4);
Z80_OPCODE_2(LD, L, D,              ({ 0x6A                                         }),  4);
Z80_OPCODE_2(LD, A, E,              ({ 0x7B                                         }),  4);
Z80_OPCODE_2(LD, B, E,              ({ 0x43                                         }),  4);
Z80_OPCODE_2(LD, C, E,              ({ 0x4B                                         }),  4);
Z80_OPCODE_2(LD, D, E,              ({ 0x53                                         }),  4);
Z80_OPCODE_2(LD, E, E,              ({ 0x5B                                         }),  4);
Z80_OPCODE_2(LD, H, E,              ({ 0x63                                         }),  4);
Z80_OPCODE_2(LD, L, E,              ({ 0x6B                                         }),  4);
Z80_OPCODE_2(LD, A, H,              ({ 0x7C                                         }),  4);
Z80_OPCODE_2(LD, B, H,              ({ 0x44                                         }),  4);
Z80_OPCODE_2(LD, C, H,              ({ 0x4C                                         }),  4);
Z80_OPCODE_2(LD, D, H,              ({ 0x54                                         }),  4);
Z80_OPCODE_2(LD, E, H,              ({ 0x5C                                         }),  4);
Z80_OPCODE_2(LD, H, H,              ({ 0x64                                         }),  4);
Z80_OPCODE_2(LD, L, H,              ({ 0x6C                                         }),  4);
Z80_OPCODE_2(LD, A, L,              ({ 0x7D                                         }),  4);
Z80_OPCODE_2(LD, B, L,              ({ 0x45                                         }),  4);
Z80_OPCODE_2(LD, C, L,              ({ 0x4D                                         }),  4);
Z80_OPCODE_2(LD, D, L,              ({ 0x55                                         }),  4);
Z80_OPCODE_2(LD, E, L,              ({ 0x5D                                         }),  4);
Z80_OPCODE_2(LD, H, L,              ({ 0x65                                         }),  4);
Z80_OPCODE_2(LD, L, L,              ({ 0x6D                                         }),  4);
Z80_OPCODE_2(LD, B, IXH,            ({ 0xDD, 0x44                                   }),  8);    // undocumented
Z80_OPCODE_2(LD, B, IYH,            ({ 0xFD, 0x44                                   }),  8);    // undocumented
Z80_OPCODE_2(LD, B, IXL,            ({ 0xDD, 0x45                                   }),  8);    // undocumented
Z80_OPCODE_2(LD, B, IYL,            ({ 0xFD, 0x45                                   }),  8);    // undocumented
Z80_OPCODE_2(LD, C, IXH,            ({ 0xDD, 0x4C                                   }),  8);    // undocumented
Z80_OPCODE_2(LD, C, IYH,            ({ 0xFD, 0x4C                                   }),  8);    // undocumented
Z80_OPCODE_2(LD, C, IXL,            ({ 0xDD, 0x4D                                   }),  8);    // undocumented
Z80_OPCODE_2(LD, C, IYL,            ({ 0xFD, 0x4D                                   }),  8);    // undocumented
Z80_OPCODE_2(LD, D, IXH,            ({ 0xDD, 0x54                                   }),  8);    // undocumented
Z80_OPCODE_2(LD, D, IYH,            ({ 0xFD, 0x54                                   }),  8);    // undocumented
Z80_OPCODE_2(LD, D, IXL,            ({ 0xDD, 0x55                                   }),  8);    // undocumented
Z80_OPCODE_2(LD, D, IYL,            ({ 0xFD, 0x55                                   }),  8);    // undocumented
Z80_OPCODE_2(LD, E, IXH,            ({ 0xDD, 0x5C                                   }),  8);    // undocumented
Z80_OPCODE_2(LD, E, IYH,            ({ 0xFD, 0x5C                                   }),  8);    // undocumented
Z80_OPCODE_2(LD, E, IXL,            ({ 0xDD, 0x5D                                   }),  8);    // undocumented
Z80_OPCODE_2(LD, E, IYL,            ({ 0xFD, 0x5D                                   }),  8);    // undocumented
Z80_OPCODE_2(LD, IXH, B,            ({ 0xDD, 0x60                                   }),  8);    // undocumented
Z80_OPCODE_2(LD, IYH, B,            ({ 0xFD, 0x60                                   }),  8);    // undocumented
Z80_OPCODE_2(LD, IXH, C,            ({ 0xDD, 0x61                                   }),  8);    // undocumented
Z80_OPCODE_2(LD, IYH, C,            ({ 0xFD, 0x61                                   }),  8);    // undocumented
Z80_OPCODE_2(LD, IXH, D,            ({ 0xDD, 0x62                                   }),  8);    // undocumented
Z80_OPCODE_2(LD, IYH, D,            ({ 0xFD, 0x62                                   }),  8);    // undocumented
Z80_OPCODE_2(LD, IXH, E,            ({ 0xDD, 0x63                                   }),  8);    // undocumented
Z80_OPCODE_2(LD, IYH, E,            ({ 0xFD, 0x63                                   }),  8);    // undocumented
Z80_OPCODE_2(LD, IXH, IXH,          ({ 0xDD, 0x64                                   }),  8);    // undocumented
Z80_OPCODE_2(LD, IYH, IYH,          ({ 0xFD, 0x64                                   }),  8);    // undocumented
Z80_OPCODE_2(LD, IXH, IXL,          ({ 0xDD, 0x65                                   }),  8);    // undocumented
Z80_OPCODE_2(LD, IYH, IYL,          ({ 0xFD, 0x65                                   }),  8);    // undocumented
Z80_OPCODE_2(LD, IXH, A,            ({ 0xDD, 0x67                                   }),  8);    // undocumented
Z80_OPCODE_2(LD, IYH, A,            ({ 0xFD, 0x67                                   }),  8);    // undocumented
Z80_OPCODE_2(LD, IXL, B,            ({ 0xDD, 0x68                                   }),  8);    // undocumented
Z80_OPCODE_2(LD, IYL, B,            ({ 0xFD, 0x68                                   }),  8);    // undocumented
Z80_OPCODE_2(LD, IXL, C,            ({ 0xDD, 0x69                                   }),  8);    // undocumented
Z80_OPCODE_2(LD, IYL, C,            ({ 0xFD, 0x69                                   }),  8);    // undocumented
Z80_OPCODE_2(LD, IXL, D,            ({ 0xDD, 0x6A                                   }),  8);    // undocumented
Z80_OPCODE_2(LD, IYL, D,            ({ 0xFD, 0x6A                                   }),  8);    // undocumented
Z80_OPCODE_2(LD, IXL, E,            ({ 0xDD, 0x6B                                   }),  8);    // undocumented
Z80_OPCODE_2(LD, IYL, E,            ({ 0xFD, 0x6B                                   }),  8);    // undocumented
Z80_OPCODE_2(LD, IXL, IXH,          ({ 0xDD, 0x6C                                   }),  8);    // undocumented
Z80_OPCODE_2(LD, IYL, IYH,          ({ 0xFD, 0x6C                                   }),  8);    // undocumented
Z80_OPCODE_2(LD, IXL, IXL,          ({ 0xDD, 0x6D                                   }),  8);    // undocumented
Z80_OPCODE_2(LD, IYL, IYL,          ({ 0xFD, 0x6D                                   }),  8);    // undocumented
Z80_OPCODE_2(LD, IXL, A,            ({ 0xDD, 0x6F                                   }),  8);    // undocumented
Z80_OPCODE_2(LD, IYL, A,            ({ 0xFD, 0x6F                                   }),  8);    // undocumented
Z80_OPCODE_2(LD, A, IXH,            ({ 0xDD, 0x7C                                   }),  8);    // undocumented
Z80_OPCODE_2(LD, A, IYH,            ({ 0xFD, 0x7C                                   }),  8);    // undocumented
Z80_OPCODE_2(LD, A, IXL,            ({ 0xDD, 0x7D                                   }),  8);    // undocumented
Z80_OPCODE_2(LD, A, IYL,            ({ 0xFD, 0x7D                                   }),  8);    // undocumented
Z80_OPCODE_2(LD, SP, HL,            ({ 0xF9                                         }),  6);
Z80_OPCODE_2(LD, SP, IX,            ({ 0xDD, 0xF9                                   }), 10);
Z80_OPCODE_2(LD, SP, IY,            ({ 0xFD, 0xF9                                   }), 10);
Z80_OPCODE_0(LDD,                   ({ 0xED, 0xA8                                   }), 16);
Z80_OPCODE_0(LDDR,                  ({ 0xED, 0xB8                                   }), TStates(21, 16));
Z80_OPCODE_0(LDI,                   ({ 0xED, 0xA0                                   }), 16);
Z80_OPCODE_0(LDIR,                  ({ 0xED, 0xB0                                   }), TStates(21, 17));
Z80_OPCODE_0(NEG,                   ({ 0xED, 0x44                                   }),  8);
Z80_OPCODE_0(NOP,                   ({ 0x00                                         }),  4);
Z80_OPCODE_1(OR, A,                 ({ 0xB7                                         }),  4);
Z80_OPCODE_1(OR, B,                 ({ 0xB0                                         }),  4);
Z80_OPCODE_1(OR, C,                 ({ 0xB1                                         }),  4);
Z80_OPCODE_1(OR, D,                 ({ 0xB2                                         }),  4);
Z80_OPCODE_1(OR, E,                 ({ 0xB3                                         }),  4);
Z80_OPCODE_1(OR, H,                 ({ 0xB4                                         }),  4);
Z80_OPCODE_1(OR, L,                 ({ 0xB5                                         }),  4);
Z80_OPCODE_1(OR, byte,              ({ 0xF6, OP1                                    }),  7);
Z80_OPCODE_1(OR, memHL,             ({ 0xB6                                         }),  7);
Z80_OPCODE_1(OR, IX_byte,           ({ 0xDD, 0xB6, OP1                              }), 19);
Z80_OPCODE_1(OR, IY_byte,           ({ 0xFD, 0xB6, OP1                              }), 19);
Z80_OPCODE_1(OR, IXH,               ({ 0xDD, 0xB4                                   }),  8);    // undocumented
Z80_OPCODE_1(OR, IYH,               ({ 0xFD, 0xB4                                   }),  8);    // undocumented
Z80_OPCODE_1(OR, IXL,               ({ 0xDD, 0xB5                                   }),  8);    // undocumented
Z80_OPCODE_1(OR, IYL,               ({ 0xFD, 0xB5                                   }),  8);    // undocumented
Z80_OPCODE_0(OTDR,                  ({ 0xED, 0xBB                                   }), TStates(21, 16));
Z80_OPCODE_0(OTIR,                  ({ 0xED, 0xB3                                   }), TStates(21, 16));
Z80_OPCODE_2(OUT, portAddr, A,      ({ 0xD3, OP1                                    }), 11);
Z80_OPCODE_2(OUT, portC, A,         ({ 0xED, 0x79                                   }), 12);
Z80_OPCODE_2(OUT, portC, B,         ({ 0xED, 0x41                                   }), 12);
Z80_OPCODE_2(OUT, portC, C,         ({ 0xED, 0x49                                   }), 12);
Z80_OPCODE_2(OUT, portC, D,         ({ 0xED, 0x51                                   }), 12);
Z80_OPCODE_2(OUT, portC, E,         ({ 0xED, 0x59                                   }), 12);
Z80_OPCODE_2(OUT, portC, H,         ({ 0xED, 0x61                                   }), 12);
Z80_OPCODE_2(OUT, portC, L,         ({ 0xED, 0x69                                   }), 12);
Z80_OPCODE_0(OUTD,                  ({ 0xED, 0xAB                                   }), 16);
Z80_OPCODE_0(OUTI,                  ({ 0xED, 0xA3                                   }), 16);
Z80_OPCODE_1(POP, IX,               ({ 0xDD, 0xE1                                   }), 14);
Z80_OPCODE_1(POP, IY,               ({ 0xFD, 0xE1                                   }), 14);
Z80_OPCODE_1(POP, BC,               ({ 0xC1                                         }), 10);
Z80_OPCODE_1(POP, DE,               ({ 0xD1                                         }), 10);
Z80_OPCODE_1(POP, HL,               ({ 0xE1                                         }), 10);
Z80_OPCODE_1(POP, AF,               ({ 0xF1                                         }), 10);
Z80_OPCODE_1(PUSH, IX,              ({ 0xDD, 0xE5                                   }), 15);
Z80_OPCODE_1(PUSH, IY,              ({ 0xFD, 0xE5                                   }), 15);
Z80_OPCODE_1(PUSH, BC,              ({ 0xC5                                         }), 11);
Z80_OPCODE_1(PUSH, DE,              ({ 0xD5                                         }), 11);
Z80_OPCODE_1(PUSH, HL,              ({ 0xE5                                         }), 11);
Z80_OPCODE_1(PUSH, AF,              ({ 0xF5                                         }), 11);
Z80_OPCODE_2(RES, bit, A,           ({ 0xCB, OP1V(0x87)                             }),  8);
Z80_OPCODE_2(RES, bit, B,           ({ 0xCB, OP1V(0x80)                             }),  8);
Z80_OPCODE_2(RES, bit, C,           ({ 0xCB, OP1V(0x81)                             }),  8);
Z80_OPCODE_2(RES, bit, D,           ({ 0xCB, OP1V(0x82)                             }),  8);
Z80_OPCODE_2(RES, bit, E,           ({ 0xCB, OP1V(0x83)                             }),  8);
Z80_OPCODE_2(RES, bit, H,           ({ 0xCB, OP1V(0x84)                             }),  8);
Z80_OPCODE_2(RES, bit, L,           ({ 0xCB, OP1V(0x85)                             }),  8);
Z80_OPCODE_2(RES, bit, memHL,       ({ 0xCB, OP1V(0x86)                             }), 15);
Z80_OPCODE_2(RES, bit, IX_byte,     ({ 0xDD, 0xCB, OP2, OP1V(0x86)                  }), 23);
Z80_OPCODE_2(RES, bit, IY_byte,     ({ 0xFD, 0xCB, OP2, OP1V(0x86)                  }), 23);
Z80_OPCODE_0(RET,                   ({ 0xC9                                         }), 10);
Z80_OPCODE_1(RET, flagC,            ({ 0xD8                                         }), TStates(11, 5));
Z80_OPCODE_1(RET, flagM,            ({ 0xF8                                         }), TStates(11, 5));
Z80_OPCODE_1(RET, flagNC,           ({ 0xD0                                         }), TStates(11, 5));
Z80_OPCODE_1(RET, flagNZ,           ({ 0xC0                                         }), TStates(11, 5));
Z80_OPCODE_1(RET, flagP,            ({ 0xF0                                         }), TStates(11, 5));
Z80_OPCODE_1(RET, flagPE,           ({ 0xE8                                         }), TStates(11, 5));
Z80_OPCODE_1(RET, flagPO,           ({ 0xE0                                         }), TStates(11, 5));
Z80_OPCODE_1(RET, flagZ,            ({ 0xC8                                         }), TStates(11, 5));
Z80_OPCODE_0(RETI,                  ({ 0xED, 0x4D                                   }), 14);
Z80_OPCODE_0(RETN,                  ({ 0xED, 0x45                                   }), 14);
Z80_OPCODE_1(RL, A,                 ({ 0xCB, 0x17                                   }),  8);
Z80_OPCODE_1(RL, B,                 ({ 0xCB, 0x10                                   }),  8);
Z80_OPCODE_1(RL, C,                 ({ 0xCB, 0x11                                   }),  8);
Z80_OPCODE_1(RL, D,                 ({ 0xCB, 0x12                                   }),  8);
Z80_OPCODE_1(RL, E,                 ({ 0xCB, 0x13                                   }),  8);
Z80_OPCODE_1(RL, H,                 ({ 0xCB, 0x14                                   }),  8);
Z80_OPCODE_1(RL, L,                 ({ 0xCB, 0x15                                   }),  8);
Z80_OPCODE_1(RL, memHL,             ({ 0xCB, 0x16                                   }), 15);
Z80_OPCODE_1(RL, IX_byte,           ({ 0xDD, 0xCB, OP1, 0x16                        }), 23);
Z80_OPCODE_1(RL, IY_byte,           ({ 0xFD, 0xCB, OP1, 0x16                        }), 23);
Z80_OPCODE_0(RLA,                   ({ 0x17                                         }),  4);
Z80_OPCODE_1(RLC, A,                ({ 0xCB, 0x07                                   }),  8);
Z80_OPCODE_1(RLC, B,                ({ 0xCB, 0x00                                   }),  8);
Z80_OPCODE_1(RLC, C,                ({ 0xCB, 0x01                                   }),  8);
Z80_OPCODE_1(RLC, D,                ({ 0xCB, 0x02                                   }),  8);
Z80_OPCODE_1(RLC, E,                ({ 0xCB, 0x03                                   }),  8);
Z80_OPCODE_1(RLC, H,                ({ 0xCB, 0x04                                   }),  8);
Z80_OPCODE_1(RLC, L,                ({ 0xCB, 0x05                                   }),  8);
Z80_OPCODE_1(RLC, memHL,            ({ 0xCB, 0x06                                   }), 15);
Z80_OPCODE_1(RLC, IX_byte,          ({ 0xDD, 0xCB, OP1, 0x06                        }), 23);
Z80_OPCODE_1(RLC, IY_byte,          ({ 0xFD, 0xCB, OP1, 0x06                        }), 23);
Z80_OPCODE_0(RLCA,                  ({ 0x07                                         }),  4);
Z80_OPCODE_0(RLD,                   ({ 0xED, 0x6F                                   }), 18);
Z80_OPCODE_1(RR, A,                 ({ 0xCB, 0x1F                                   }),  8);
Z80_OPCODE_1(RR, B,                 ({ 0xCB, 0x18                                   }),  8);
Z80_OPCODE_1(RR, C,                 ({ 0xCB, 0x19                                   }),  8);
Z80_OPCODE_1(RR, D,                 ({ 0xCB, 0x1A                                   }),  8);
Z80_OPCODE_1(RR, E,                 ({ 0xCB, 0x1B                                   }),  8);
Z80_OPCODE_1(RR, H,                 ({ 0xCB, 0x1C                                   }),  8);
Z80_OPCODE_1(RR, L,                 ({ 0xCB, 0x1D                                   }),  8);
Z80_OPCODE_1(RR, memHL,             ({ 0xCB, 0x1E                                   }), 15);
Z80_OPCODE_1(RR, IX_byte,           ({ 0xDD, 0xCB, OP1, 0x1E                        }), 23);
Z80_OPCODE_1(RR, IY_byte,           ({ 0xFD, 0xCB, OP1, 0x1E                        }), 23);
Z80_OPCODE_0(RRA,                   ({ 0x1F                                         }),  4);
Z80_OPCODE_1(RRC, A,                ({ 0xCB, 0x0F                                   }),  8);
Z80_OPCODE_1(RRC, B,                ({ 0xCB, 0x08                                   }),  8);
Z80_OPCODE_1(RRC, C,                ({ 0xCB, 0x09                                   }),  8);
Z80_OPCODE_1(RRC, D,                ({ 0xCB, 0x0A                                   }),  8);
Z80_OPCODE_1(RRC, E,                ({ 0xCB, 0x0B                                   }),  8);
Z80_OPCODE_1(RRC, H,                ({ 0xCB, 0x0C                                   }),  8);
Z80_OPCODE_1(RRC, L,                ({ 0xCB, 0x0D                                   }),  8);
Z80_OPCODE_1(RRC, memHL,            ({ 0xCB, 0x0E                                   }), 15);
Z80_OPCODE_1(RRC, IX_byte,          ({ 0xDD, 0xCB, OP1, 0x0E                        }), 23);
Z80_OPCODE_1(RRC, IY_byte,          ({ 0xFD, 0xCB, OP1, 0x0E                        }), 23);
Z80_OPCODE_0(RRCA,                  ({ 0x0F                                         }),  4);
Z80_OPCODE_0(RRD,                   ({ 0xED, 0x67                                   }), 18);
Z80_OPCODE_1(RST, rstIndex,         ({ OP1V(0xC7)                                   }), 11);
Z80_OPCODE_2(SBC, A, A,             ({ 0x9F                                         }),  4);
Z80_OPCODE_2(SBC, A, B,             ({ 0x98                                         }),  4);
Z80_OPCODE_2(SBC, A, C,             ({ 0x99                                         }),  4);
Z80_OPCODE_2(SBC, A, D,             ({ 0x9A                                         }),  4);
Z80_OPCODE_2(SBC, A, E,             ({ 0x9B                                         }),  4);
Z80_OPCODE_2(SBC, A, H,             ({ 0x9C                                         }),  4);
Z80_OPCODE_2(SBC, A, L,             ({ 0x9D                                         }),  4);
Z80_OPCODE_2(SBC, A, byte,          ({ 0xDE, OP2                                    }),  7);
Z80_OPCODE_2(SBC, A, memHL,         ({ 0x9E                                         }),  7);
Z80_OPCODE_2(SBC, A, IX_byte,       ({ 0xDD, 0x9E, OP2                              }), 19);
Z80_OPCODE_2(SBC, A, IY_byte,       ({ 0xFD, 0x9E, OP2                              }), 19);
Z80_OPCODE_2(SBC, HL, BC,           ({ 0xED, 0x42                                   }), 15);
Z80_OPCODE_2(SBC, HL, DE,           ({ 0xED, 0x52                                   }), 15);
Z80_OPCODE_2(SBC, HL, HL,           ({ 0xED, 0x62                                   }), 15);
Z80_OPCODE_2(SBC, HL, SP,           ({ 0xED, 0x72                                   }), 15);
Z80_OPCODE_2(SBC, A, IXH,           ({ 0xDD, 0x9C                                   }),  8);    // undocumented
Z80_OPCODE_2(SBC, A, IYH,           ({ 0xFD, 0x9C                                   }),  8);    // undocumented
Z80_OPCODE_2(SBC, A, IXL,           ({ 0xDD, 0x9D                                   }),  8);    // undocumented
Z80_OPCODE_2(SBC, A, IYL,           ({ 0xFD, 0x9D                                   }),  8);    // undocumented
Z80_OPCODE_0(SCF,                   ({ 0x37                                         }),  4);
Z80_OPCODE_2(SET, bit, A,           ({ 0xCB, OP1V(0xC7)                             }),  8);
Z80_OPCODE_2(SET, bit, B,           ({ 0xCB, OP1V(0xC0)                             }),  8);
Z80_OPCODE_2(SET, bit, C,           ({ 0xCB, OP1V(0xC1)                             }),  8);
Z80_OPCODE_2(SET, bit, D,           ({ 0xCB, OP1V(0xC2)                             }),  8);
Z80_OPCODE_2(SET, bit, E,           ({ 0xCB, OP1V(0xC3)                             }),  8);
Z80_OPCODE_2(SET, bit, H,           ({ 0xCB, OP1V(0xC4)                             }),  8);
Z80_OPCODE_2(SET, bit, L,           ({ 0xCB, OP1V(0xC5)                             }),  8);
Z80_OPCODE_2(SET, bit, memHL,       ({ 0xCB, OP1V(0xC6)                             }), 15);
Z80_OPCODE_2(SET, bit, IX_byte,     ({ 0xDD, 0xCB, OP2, OP1V(0xC6)                  }), 23);
Z80_OPCODE_2(SET, bit, IY_byte,     ({ 0xFD, 0xCB, OP2, OP1V(0xC6)                  }), 23);
Z80_OPCODE_1(SLA, A,                ({ 0xCB, 0x27                                   }),  8);
Z80_OPCODE_1(SLA, B,                ({ 0xCB, 0x20                                   }),  8);
Z80_OPCODE_1(SLA, C,                ({ 0xCB, 0x21                                   }),  8);
Z80_OPCODE_1(SLA, D,                ({ 0xCB, 0x22                                   }),  8);
Z80_OPCODE_1(SLA, E,                ({ 0xCB, 0x23                                   }),  8);
Z80_OPCODE_1(SLA, H,                ({ 0xCB, 0x24                                   }),  8);
Z80_OPCODE_1(SLA, L,                ({ 0xCB, 0x25                                   }),  8);
Z80_OPCODE_1(SLA, memHL,            ({ 0xCB, 0x26                                   }), 15);
Z80_OPCODE_1(SLA, IX_byte,          ({ 0xDD, 0xCB, OP1, 0x26                        }), 23);
Z80_OPCODE_1(SLA, IY_byte,          ({ 0xFD, 0xCB, OP1, 0x26                        }), 23);
Z80_OPCODE_1(SLL, B,                ({ 0xCB, 0x30                                   }),  8);    // undocumented
Z80_OPCODE_1(SLL, C,                ({ 0xCB, 0x31                                   }),  8);    // undocumented
Z80_OPCODE_1(SLL, D,                ({ 0xCB, 0x32                                   }),  8);    // undocumented
Z80_OPCODE_1(SLL, E,                ({ 0xCB, 0x33                                   }),  8);    // undocumented
Z80_OPCODE_1(SLL, H,                ({ 0xCB, 0x34                                   }),  8);    // undocumented
Z80_OPCODE_1(SLL, L,                ({ 0xCB, 0x35                                   }),  8);    // undocumented
Z80_OPCODE_1(SLL, memHL,            ({ 0xCB, 0x36                                   }), 15);    // undocumented
Z80_OPCODE_1(SLL, IX_byte,          ({ 0xDD, 0xCB, OP1, 0x36                        }), 23);    // undocumented
Z80_OPCODE_1(SLL, IY_byte,          ({ 0xFD, 0xCB, OP1, 0x36                        }), 23);    // undocumented
Z80_OPCODE_1(SLL, A,                ({ 0xCB, 0x37                                   }),  8);    // undocumented
Z80_OPCODE_1(SRA, A,                ({ 0xCB, 0x2F                                   }),  8);
Z80_OPCODE_1(SRA, B,                ({ 0xCB, 0x28                                   }),  8);
Z80_OPCODE_1(SRA, C,                ({ 0xCB, 0x29                                   }),  8);
Z80_OPCODE_1(SRA, D,                ({ 0xCB, 0x2A                                   }),  8);
Z80_OPCODE_1(SRA, E,                ({ 0xCB, 0x2B                                   }),  8);
Z80_OPCODE_1(SRA, H,                ({ 0xCB, 0x2C                                   }),  8);
Z80_OPCODE_1(SRA, L,                ({ 0xCB, 0x2D                                   }),  8);
Z80_OPCODE_1(SRA, memHL,            ({ 0xCB, 0x2E                                   }), 15);
Z80_OPCODE_1(SRA, IX_byte,          ({ 0xDD, 0xCB, OP1, 0x2E                        }), 23);
Z80_OPCODE_1(SRA, IY_byte,          ({ 0xFD, 0xCB, OP1, 0x2E                        }), 23);
Z80_OPCODE_1(SRL, A,                ({ 0xCB, 0x3F                                   }),  8);
Z80_OPCODE_1(SRL, B,                ({ 0xCB, 0x38                                   }),  8);
Z80_OPCODE_1(SRL, C,                ({ 0xCB, 0x39                                   }),  8);
Z80_OPCODE_1(SRL, D,                ({ 0xCB, 0x3A                                   }),  8);
Z80_OPCODE_1(SRL, E,                ({ 0xCB, 0x3B                                   }),  8);
Z80_OPCODE_1(SRL, H,                ({ 0xCB, 0x3C                                   }),  8);
Z80_OPCODE_1(SRL, L,                ({ 0xCB, 0x3D                                   }),  8);
Z80_OPCODE_1(SRL, memHL,            ({ 0xCB, 0x3E                                   }), 15);
Z80_OPCODE_1(SRL, IX_byte,          ({ 0xDD, 0xCB, OP1, 0x3E                        }), 23);
Z80_OPCODE_1(SRL, IY_byte,          ({ 0xFD, 0xCB, OP1, 0x3E                        }), 23);
Z80_OPCODE_1(SUB, A,                ({ 0x97                                         }),  4);
Z80_OPCODE_1(SUB, B,                ({ 0x90                                         }),  4);
Z80_OPCODE_1(SUB, C,                ({ 0x91                                         }),  4);
Z80_OPCODE_1(SUB, D,                ({ 0x92                                         }),  4);
Z80_OPCODE_1(SUB, E,                ({ 0x93                                         }),  4);
Z80_OPCODE_1(SUB, H,                ({ 0x94                                         }),  4);
Z80_OPCODE_1(SUB, L,                ({ 0x95                                         }),  4);
Z80_OPCODE_1(SUB, byte,             ({ 0xD6, OP1                                    }),  7);
Z80_OPCODE_1(SUB, memHL,            ({ 0x96                                         }),  7);
Z80_OPCODE_1(SUB, IXH,              ({ 0xDD, 0x94                                   }),  8);    // undocumented
Z80_OPCODE_1(SUB, IYH,              ({ 0xFD, 0x94                                   }),  8);    // undocumented
Z80_OPCODE_1(SUB, IXL,              ({ 0xDD, 0x95                                   }),  8);    // undocumented
Z80_OPCODE_1(SUB, IYL,              ({ 0xFD, 0x95                                   }),  8);    // undocumented
Z80_OPCODE_1(SUB, IX_byte,          ({ 0xDD, 0x96, OP1                              }), 19);
Z80_OPCODE_1(SUB, IY_byte,          ({ 0xFD, 0x96, OP1                              }), 19);
Z80_OPCODE_1(XOR, A,                ({ 0xAF                                         }),  4);
Z80_OPCODE_1(XOR, B,                ({ 0xA8                                         }),  4);
Z80_OPCODE_1(XOR, C,                ({ 0xA9                                         }),  4);
Z80_OPCODE_1(XOR, D,                ({ 0xAA                                         }),  4);
Z80_OPCODE_1(XOR, E,                ({ 0xAB                                         }),  4);
Z80_OPCODE_1(XOR, H,                ({ 0xAC                                         }),  4);
Z80_OPCODE_1(XOR, L,                ({ 0xAD                                         }),  4);
Z80_OPCODE_1(XOR, byte,             ({ 0xEE, OP1                                    }),  7);
Z80_OPCODE_1(XOR, memHL,            ({ 0xAE                                         }),  7);
Z80_OPCODE_1(XOR, IX_byte,          ({ 0xDD, 0xAE, OP1                              }), 19);
Z80_OPCODE_1(XOR, IY_byte,          ({ 0xFD, 0xAE, OP1                              }), 19);
Z80_OPCODE_1(XOR, IXH,              ({ 0xDD, 0xAC                                   }),  8);    // undocumented
Z80_OPCODE_1(XOR, IYH,              ({ 0xFD, 0xAC                                   }),  8);    // undocumented
Z80_OPCODE_1(XOR, IXL,              ({ 0xDD, 0xAD                                   }),  8);    // undocumented
Z80_OPCODE_1(XOR, IYL,              ({ 0xFD, 0xAD                                   }),  8);    // undocumented
