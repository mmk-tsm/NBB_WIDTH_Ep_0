///////////////////////////////////////////////////////
// ModbusTables.h
//
//
// M.Parks                                                                      08-02-2000
// First pass.
// P.Smith      10-05-2001
// Changes at Lines 402,3 - register no.s changed.
// M. McKiernan 12-06-2001 Added extra entry to each component block in production summary (set wt)
// M. McKiernan 08-03-2002 Changed definition for Non-Standard Configuration for batch L. 121.
// P.Smith      04-09-2002 modified to allow weight per meter download as 2 word instead of 1.
// M. McKiernan 17-10-2002 Added 2 extra entries in calibration 125,126, for Off-line type and alarm deadband
//					  Used up two unused parameters- hence table did not need resequencing.
// 
// P.Smith                      23/1/06
// first pass at netburner hardware conversion.

///////////////////////////////////////////////////////

//*****************************************************************************
// MACROS
//*****************************************************************************
#ifndef __MODBUSTABLES_H__
#define __MODBUSTABLES_H__


//*****************************************************************************
// INCLUDES
//*****************************************************************************
#include "ModbusIndices.h"

// Data is stored in the form - Register number, decimal places
// and int/long (0/1) indicator
#define TABLE_REGISTER_INDEX            0
#define TABLE_DECIMAL_INDEX                     1
#define TABLE_SIZE_INDEX                        2

// Indicators for ints and longs
#define TABLE_REGISTER_SIZE_INT         0
#define TABLE_REGISTER_SIZE_LONG                1


// Batch Blender 
const int arrnBatchBlenderModbusTable[BATCH_READ_TABLE_SIZE][3] = {
        { 0, 0, TABLE_REGISTER_SIZE_INT },              // Start of Common data, not actually modbus data
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },

        { 0, 2, TABLE_REGISTER_SIZE_INT },              // Start of setpoints (20)
        { 1, 2, TABLE_REGISTER_SIZE_INT },
        { 2, 2, TABLE_REGISTER_SIZE_INT },
        { 3, 2, TABLE_REGISTER_SIZE_INT },
        { 4, 2, TABLE_REGISTER_SIZE_INT },
        { 5, 2, TABLE_REGISTER_SIZE_INT },
        { 6, 2, TABLE_REGISTER_SIZE_INT },
        { 7, 2, TABLE_REGISTER_SIZE_INT },
        { 11, 0, TABLE_REGISTER_SIZE_INT },
        { 12, 2, TABLE_REGISTER_SIZE_LONG },             // weight per meter setpoint (29)
        { 14, 1, TABLE_REGISTER_SIZE_INT },
        { 15, 2, TABLE_REGISTER_SIZE_INT },
        { 16, 1, TABLE_REGISTER_SIZE_INT },
        { 17, 0, TABLE_REGISTER_SIZE_INT },
        { 18, 0, TABLE_REGISTER_SIZE_INT },
        { 21, 0, TABLE_REGISTER_SIZE_INT },
        { 22, 0, TABLE_REGISTER_SIZE_INT },
        { 23, 0, TABLE_REGISTER_SIZE_LONG },
        { 25, 0, TABLE_REGISTER_SIZE_LONG },
        { 27, 0, TABLE_REGISTER_SIZE_INT },
        { 35, 0, TABLE_REGISTER_SIZE_INT },
        { 43, 0, TABLE_REGISTER_SIZE_INT },
        { 51, 0, TABLE_REGISTER_SIZE_INT },
        { 52, 0, TABLE_REGISTER_SIZE_INT },
        { 53, 0, TABLE_REGISTER_SIZE_INT },
        { 54, 0, TABLE_REGISTER_SIZE_INT },
        { 71, 0, TABLE_REGISTER_SIZE_INT },             // End of setpoints     (46)


        { 90, 0, TABLE_REGISTER_SIZE_INT },             // Start of calibration (47)
        { 91, 0, TABLE_REGISTER_SIZE_INT },
        { 92, 0, TABLE_REGISTER_SIZE_INT },
        { 93, 2, TABLE_REGISTER_SIZE_INT },
        { 97, 0, TABLE_REGISTER_SIZE_INT },
        { 98, 0, TABLE_REGISTER_SIZE_INT },
        { 99, 0, TABLE_REGISTER_SIZE_INT },
        { 100, 1, TABLE_REGISTER_SIZE_INT },
        { 105, 0, TABLE_REGISTER_SIZE_INT },
        { 106, 0, TABLE_REGISTER_SIZE_INT },
        { 107, 0, TABLE_REGISTER_SIZE_INT },
        { 108, 0, TABLE_REGISTER_SIZE_INT },
        { 109, 0, TABLE_REGISTER_SIZE_INT },
        { 110, 0, TABLE_REGISTER_SIZE_INT },
        { 111, 0, TABLE_REGISTER_SIZE_INT },
        { 112, 0, TABLE_REGISTER_SIZE_INT },
        { 113, 0, TABLE_REGISTER_SIZE_INT },
        { 114, 0, TABLE_REGISTER_SIZE_INT },
        { 115, 0, TABLE_REGISTER_SIZE_INT },
        { 116, 0, TABLE_REGISTER_SIZE_INT },
        { 117, 0, TABLE_REGISTER_SIZE_INT },
        { 118, 0, TABLE_REGISTER_SIZE_INT },
        { 119, 0, TABLE_REGISTER_SIZE_INT },
        { 120, 0, TABLE_REGISTER_SIZE_INT },
        { 121, 0, TABLE_REGISTER_SIZE_INT },
        { 122, 0, TABLE_REGISTER_SIZE_INT },
        { 123, 0, TABLE_REGISTER_SIZE_INT },  //Stall check

        { 124, 0, TABLE_REGISTER_SIZE_INT },    // Delay to level sensor alarm
        { 125, 0, TABLE_REGISTER_SIZE_INT },			// Off-line type
        { 126, 0, TABLE_REGISTER_SIZE_INT },			// alarm deadband.

        { 128, 0, TABLE_REGISTER_SIZE_INT },		// 2 unused for future expansion
        { 129, 0, TABLE_REGISTER_SIZE_INT },

//      { 130, 1, TABLE_REGISTER_SIZE_INT },    // non standard conf.
        { 130, 0, TABLE_REGISTER_SIZE_INT },    // non standard conf. - MMK no decimal places.
        { 131, 0, TABLE_REGISTER_SIZE_INT },
        { 132, 0, TABLE_REGISTER_SIZE_INT },
        { 133, 0, TABLE_REGISTER_SIZE_INT },
        { 134, 0, TABLE_REGISTER_SIZE_INT },
        { 135, 0, TABLE_REGISTER_SIZE_INT },
        { 136, 0, TABLE_REGISTER_SIZE_INT },
        { 137, 0, TABLE_REGISTER_SIZE_INT },
        { 138, 0, TABLE_REGISTER_SIZE_INT },    // comp 8 config.
        { 145, 3, TABLE_REGISTER_SIZE_INT },    //cal wt
        { 147, 1, TABLE_REGISTER_SIZE_INT },
        { 148, 0, TABLE_REGISTER_SIZE_INT },
        { 149, 0, TABLE_REGISTER_SIZE_INT },
        { 150, 0, TABLE_REGISTER_SIZE_LONG },   //Tare counts           // Was End of calibration (87)

        { 152, 0, TABLE_REGISTER_SIZE_LONG },           // Weight calibration const
        { 154, 0, TABLE_REGISTER_SIZE_INT },            // 4 unused parameters for future expansion
        { 155, 0, TABLE_REGISTER_SIZE_INT },
        { 156, 0, TABLE_REGISTER_SIZE_INT },
        { 157, 0, TABLE_REGISTER_SIZE_INT },            //New End of calibration (mmk 17052001) (97)

// PIB should be added in here

        { 270, 0, TABLE_REGISTER_SIZE_INT },            // Start of flag controls (98)
        { 271, 0, TABLE_REGISTER_SIZE_INT },
        { 272, 0, TABLE_REGISTER_SIZE_INT },
        { 273, 0, TABLE_REGISTER_SIZE_INT },
        { 274, 0, TABLE_REGISTER_SIZE_INT },
        { 275, 0, TABLE_REGISTER_SIZE_INT },
        { 276, 0, TABLE_REGISTER_SIZE_INT },
        { 277, 0, TABLE_REGISTER_SIZE_INT },
        { 278, 0, TABLE_REGISTER_SIZE_INT },            // End of flag controls (106)

        { 285, 0, TABLE_REGISTER_SIZE_INT },            // Start of time (107)
        { 286, 0, TABLE_REGISTER_SIZE_INT },
        { 287, 0, TABLE_REGISTER_SIZE_INT },
        { 288, 0, TABLE_REGISTER_SIZE_INT },            // End of time (110)

        { 400, 2, TABLE_REGISTER_SIZE_LONG },           // Start of summary (111)
        { 402, 2, TABLE_REGISTER_SIZE_LONG },
        { 404, 2, TABLE_REGISTER_SIZE_INT },
        { 405, 0, TABLE_REGISTER_SIZE_INT },
        { 406, 0, TABLE_REGISTER_SIZE_INT },
        { 407, 0, TABLE_REGISTER_SIZE_LONG },
        { 409, 0, TABLE_REGISTER_SIZE_INT },
        { 410, 0, TABLE_REGISTER_SIZE_INT },
        { 411, 0, TABLE_REGISTER_SIZE_INT },
        { 412, 0, TABLE_REGISTER_SIZE_INT },
        { 413, 0, TABLE_REGISTER_SIZE_LONG },
        { 415, 0, TABLE_REGISTER_SIZE_INT },
        { 416, 0, TABLE_REGISTER_SIZE_INT },
        { 417, 2, TABLE_REGISTER_SIZE_INT },
        { 418, 2, TABLE_REGISTER_SIZE_INT },
        { 419, 0, TABLE_REGISTER_SIZE_INT },
        { 420, 0, TABLE_REGISTER_SIZE_INT },
        { 421, 0, TABLE_REGISTER_SIZE_INT },
        { 422, 0, TABLE_REGISTER_SIZE_INT },
        { 423, 0, TABLE_REGISTER_SIZE_INT },
        { 424, 0, TABLE_REGISTER_SIZE_INT },
        { 425, 0, TABLE_REGISTER_SIZE_INT },
        { 426, 0, TABLE_REGISTER_SIZE_LONG },
        { 428, 0, TABLE_REGISTER_SIZE_LONG },
        { 430, 4, TABLE_REGISTER_SIZE_LONG },
        { 432, 0, TABLE_REGISTER_SIZE_INT },
        { 433, 0, TABLE_REGISTER_SIZE_INT },

        { 436, 4, TABLE_REGISTER_SIZE_LONG },   // extra added 120601 mmk - Component set. weight
        { 438, 2, TABLE_REGISTER_SIZE_INT },            // Component 1 (138)
        { 439, 4, TABLE_REGISTER_SIZE_LONG },
        { 441, 2, TABLE_REGISTER_SIZE_LONG },
        { 443, 2, TABLE_REGISTER_SIZE_LONG },
        { 445, 2, TABLE_REGISTER_SIZE_LONG },
        { 447, 0, TABLE_REGISTER_SIZE_INT },
        { 448, 0, TABLE_REGISTER_SIZE_LONG },
        { 450, 0, TABLE_REGISTER_SIZE_INT },
        { 452, 0, TABLE_REGISTER_SIZE_INT },
        { 455, 0, TABLE_REGISTER_SIZE_LONG },
        { 457, 0, TABLE_REGISTER_SIZE_INT },

        { 458, 4, TABLE_REGISTER_SIZE_LONG },   // extra added 120601 mmk - Component set. weight
        { 460, 2, TABLE_REGISTER_SIZE_INT },            // Component 2 (150)
        { 461, 4, TABLE_REGISTER_SIZE_LONG },
        { 463, 2, TABLE_REGISTER_SIZE_LONG },
        { 465, 2, TABLE_REGISTER_SIZE_LONG },
        { 467, 2, TABLE_REGISTER_SIZE_LONG },
        { 469, 0, TABLE_REGISTER_SIZE_INT },
        { 470, 0, TABLE_REGISTER_SIZE_LONG },
        { 472, 0, TABLE_REGISTER_SIZE_INT },
        { 474, 0, TABLE_REGISTER_SIZE_INT },
        { 477, 2, TABLE_REGISTER_SIZE_LONG },
        { 479, 0, TABLE_REGISTER_SIZE_INT },

        { 480, 4, TABLE_REGISTER_SIZE_LONG },   // extra added 120601 mmk - Component set. weight
        { 482, 2, TABLE_REGISTER_SIZE_INT },            // Component 3 (156)
        { 483, 4, TABLE_REGISTER_SIZE_LONG },
        { 485, 2, TABLE_REGISTER_SIZE_LONG },
        { 487, 2, TABLE_REGISTER_SIZE_LONG },
        { 489, 2, TABLE_REGISTER_SIZE_LONG },
        { 491, 0, TABLE_REGISTER_SIZE_INT },
        { 492, 0, TABLE_REGISTER_SIZE_LONG },
        { 494, 0, TABLE_REGISTER_SIZE_INT },
        { 496, 0, TABLE_REGISTER_SIZE_INT },
        { 499, 2, TABLE_REGISTER_SIZE_LONG },
        { 501, 0, TABLE_REGISTER_SIZE_INT },

        { 502, 4, TABLE_REGISTER_SIZE_LONG },   // extra added 120601 mmk - Component set. weight
        { 504, 2, TABLE_REGISTER_SIZE_INT },            // Component 4 (165)
        { 505, 4, TABLE_REGISTER_SIZE_LONG },
        { 507, 2, TABLE_REGISTER_SIZE_LONG },
        { 509, 2, TABLE_REGISTER_SIZE_LONG },
        { 511, 2, TABLE_REGISTER_SIZE_LONG },
        { 513, 0, TABLE_REGISTER_SIZE_INT },
        { 514, 0, TABLE_REGISTER_SIZE_LONG },
        { 516, 0, TABLE_REGISTER_SIZE_INT },
        { 518, 0, TABLE_REGISTER_SIZE_INT },
        { 521, 2, TABLE_REGISTER_SIZE_LONG },
        { 523, 0, TABLE_REGISTER_SIZE_INT },

        { 524, 4, TABLE_REGISTER_SIZE_LONG },   // extra added 120601 mmk - Component set. weight
        { 526, 2, TABLE_REGISTER_SIZE_INT },            // Component 5 (174)
        { 527, 4, TABLE_REGISTER_SIZE_LONG },
        { 529, 2, TABLE_REGISTER_SIZE_LONG },
        { 531, 2, TABLE_REGISTER_SIZE_LONG },
        { 533, 2, TABLE_REGISTER_SIZE_LONG },
        { 535, 0, TABLE_REGISTER_SIZE_INT },
        { 536, 0, TABLE_REGISTER_SIZE_LONG },
        { 538, 0, TABLE_REGISTER_SIZE_INT },
        { 540, 0, TABLE_REGISTER_SIZE_INT },
        { 543, 2, TABLE_REGISTER_SIZE_LONG },
        { 545, 0, TABLE_REGISTER_SIZE_INT },

        { 546, 4, TABLE_REGISTER_SIZE_LONG },   // extra added 120601 mmk - Component set. weight
        { 548, 2, TABLE_REGISTER_SIZE_INT },            // Component 6 (183)
        { 549, 4, TABLE_REGISTER_SIZE_LONG },
        { 551, 2, TABLE_REGISTER_SIZE_LONG },
        { 553, 2, TABLE_REGISTER_SIZE_LONG },
        { 555, 2, TABLE_REGISTER_SIZE_LONG },
        { 557, 0, TABLE_REGISTER_SIZE_INT },
        { 558, 0, TABLE_REGISTER_SIZE_LONG },
        { 560, 0, TABLE_REGISTER_SIZE_INT },
        { 562, 0, TABLE_REGISTER_SIZE_INT },
        { 565, 2, TABLE_REGISTER_SIZE_LONG },
        { 567, 0, TABLE_REGISTER_SIZE_INT },

        { 568, 4, TABLE_REGISTER_SIZE_LONG },   // extra added 120601 mmk - Component set. weight
        { 570, 2, TABLE_REGISTER_SIZE_INT },            // Component 7 (192)
        { 571, 4, TABLE_REGISTER_SIZE_LONG },
        { 573, 2, TABLE_REGISTER_SIZE_LONG },
        { 575, 2, TABLE_REGISTER_SIZE_LONG },
        { 577, 2, TABLE_REGISTER_SIZE_LONG },
        { 579, 0, TABLE_REGISTER_SIZE_INT },
        { 580, 0, TABLE_REGISTER_SIZE_LONG },
        { 582, 0, TABLE_REGISTER_SIZE_INT },
        { 584, 0, TABLE_REGISTER_SIZE_INT },
        { 587, 2, TABLE_REGISTER_SIZE_LONG },
        { 589, 0, TABLE_REGISTER_SIZE_INT },

        { 590, 4, TABLE_REGISTER_SIZE_LONG },   // extra added 120601 mmk - Component set. weight
        { 592, 2, TABLE_REGISTER_SIZE_INT },            // Component 8 (201)
        { 593, 4, TABLE_REGISTER_SIZE_LONG },
        { 595, 2, TABLE_REGISTER_SIZE_LONG },
        { 597, 2, TABLE_REGISTER_SIZE_LONG },
        { 599, 2, TABLE_REGISTER_SIZE_LONG },
        { 601, 0, TABLE_REGISTER_SIZE_INT },
        { 602, 0, TABLE_REGISTER_SIZE_LONG },
        { 604, 0, TABLE_REGISTER_SIZE_INT },
        { 606, 0, TABLE_REGISTER_SIZE_INT },
        { 609, 2, TABLE_REGISTER_SIZE_LONG },
        { 611, 0, TABLE_REGISTER_SIZE_INT },

        { 612, 0, TABLE_REGISTER_SIZE_INT },    // **extra added 120601 mmk - unused spare
        { 613, 0, TABLE_REGISTER_SIZE_INT },    // **extra added 120601 mmk - unused spare

        { 675, 0, TABLE_REGISTER_SIZE_INT },            // Front Roll report (210)
        { 676, 2, TABLE_REGISTER_SIZE_INT },
        { 677, 2, TABLE_REGISTER_SIZE_INT },
        { 678, 1, TABLE_REGISTER_SIZE_INT },
        { 679, 1, TABLE_REGISTER_SIZE_INT },
        { 680, 0, TABLE_REGISTER_SIZE_INT },
        { 681, 0, TABLE_REGISTER_SIZE_INT },
        { 682, 2, TABLE_REGISTER_SIZE_LONG },
        { 684, 2, TABLE_REGISTER_SIZE_LONG },
        { 686, 2, TABLE_REGISTER_SIZE_LONG },
        { 688, 0, TABLE_REGISTER_SIZE_INT },
        { 689, 0, TABLE_REGISTER_SIZE_LONG },
        { 691, 0, TABLE_REGISTER_SIZE_INT },
        { 699, 0, TABLE_REGISTER_SIZE_INT },
        { 707, 0, TABLE_REGISTER_SIZE_INT },
        { 715, 0, TABLE_REGISTER_SIZE_INT },
        { 716, 0, TABLE_REGISTER_SIZE_INT },
        { 717, 0, TABLE_REGISTER_SIZE_INT },
        { 718, 0, TABLE_REGISTER_SIZE_INT },
        { 719, 0, TABLE_REGISTER_SIZE_INT },
        { 720, 0, TABLE_REGISTER_SIZE_INT },
        { 721, 0, TABLE_REGISTER_SIZE_INT },
        { 722, 0, TABLE_REGISTER_SIZE_INT },
        { 723, 0, TABLE_REGISTER_SIZE_INT },
        { 724, 0, TABLE_REGISTER_SIZE_INT },
        { 726, 0, TABLE_REGISTER_SIZE_INT },
        { 727, 0, TABLE_REGISTER_SIZE_INT },

        { 728, 2, TABLE_REGISTER_SIZE_INT },            // Component 1 (237)
        { 729, 2, TABLE_REGISTER_SIZE_LONG },

        { 736, 2, TABLE_REGISTER_SIZE_INT },
        { 737, 2, TABLE_REGISTER_SIZE_LONG },

        { 744, 2, TABLE_REGISTER_SIZE_INT },
        { 745, 2, TABLE_REGISTER_SIZE_LONG },

        { 752, 2, TABLE_REGISTER_SIZE_INT },
        { 753, 2, TABLE_REGISTER_SIZE_LONG },

        { 760, 2, TABLE_REGISTER_SIZE_INT },
        { 761, 2, 1     },

        { 768, 2, TABLE_REGISTER_SIZE_INT },
        { 769, 2, TABLE_REGISTER_SIZE_LONG },

        { 776, 2, TABLE_REGISTER_SIZE_INT },
        { 777, 2, TABLE_REGISTER_SIZE_LONG },

        { 784, 2, TABLE_REGISTER_SIZE_INT },
        { 785, 2, TABLE_REGISTER_SIZE_LONG },

        { 825, 0, TABLE_REGISTER_SIZE_INT },            // Back Roll report (253)
        { 826, 2, TABLE_REGISTER_SIZE_INT },
        { 827, 2, TABLE_REGISTER_SIZE_INT },
        { 828, 1, TABLE_REGISTER_SIZE_INT },
        { 829, 1, TABLE_REGISTER_SIZE_INT },
        { 830, 0, TABLE_REGISTER_SIZE_INT },
        { 831, 0, TABLE_REGISTER_SIZE_INT },
        { 832, 2, TABLE_REGISTER_SIZE_LONG },
        { 834, 2, TABLE_REGISTER_SIZE_LONG },
        { 836, 2, TABLE_REGISTER_SIZE_LONG },
        { 838, 0, TABLE_REGISTER_SIZE_INT },
        { 839, 0, TABLE_REGISTER_SIZE_LONG },
        { 841, 0, TABLE_REGISTER_SIZE_INT },
        { 849, 0, TABLE_REGISTER_SIZE_INT },
        { 857, 0, TABLE_REGISTER_SIZE_INT },
        { 865, 0, TABLE_REGISTER_SIZE_INT },
        { 866, 0, TABLE_REGISTER_SIZE_INT },
        { 867, 0, TABLE_REGISTER_SIZE_INT },
        { 868, 0, TABLE_REGISTER_SIZE_INT },
        { 869, 0, TABLE_REGISTER_SIZE_INT },
        { 870, 0, TABLE_REGISTER_SIZE_INT },
        { 871, 0, TABLE_REGISTER_SIZE_INT },
        { 872, 0, TABLE_REGISTER_SIZE_INT },
        { 873, 0, TABLE_REGISTER_SIZE_INT },
        { 874, 0, TABLE_REGISTER_SIZE_INT },
        { 876, 0, TABLE_REGISTER_SIZE_INT },
        { 877, 0, TABLE_REGISTER_SIZE_INT },

        { 878, 2, TABLE_REGISTER_SIZE_INT },            // Component 1 (280)
        { 879, 2, TABLE_REGISTER_SIZE_LONG },

        { 886, 2, TABLE_REGISTER_SIZE_INT },
        { 887, 2, TABLE_REGISTER_SIZE_LONG },

        { 894, 2, TABLE_REGISTER_SIZE_INT },
        { 895, 2, TABLE_REGISTER_SIZE_LONG },

        { 902, 2, TABLE_REGISTER_SIZE_INT },
        { 903, 2, TABLE_REGISTER_SIZE_LONG },

        { 910, 2, TABLE_REGISTER_SIZE_INT },
        { 911, 2, TABLE_REGISTER_SIZE_LONG },

        { 918, 2, TABLE_REGISTER_SIZE_INT },
        { 919, 2, TABLE_REGISTER_SIZE_LONG },

        { 926, 2, TABLE_REGISTER_SIZE_INT },
        { 927, 2, TABLE_REGISTER_SIZE_LONG },

        { 934, 2, TABLE_REGISTER_SIZE_INT },
        { 935, 2, TABLE_REGISTER_SIZE_LONG },

        { 975, 0, TABLE_REGISTER_SIZE_INT },            // Order Report (296)
        { 976, 2, TABLE_REGISTER_SIZE_INT },
        { 977, 2, TABLE_REGISTER_SIZE_INT },
        { 978, 1, TABLE_REGISTER_SIZE_INT },
        { 979, 1, TABLE_REGISTER_SIZE_INT },
        { 980, 0, TABLE_REGISTER_SIZE_INT },
        { 981, 0, TABLE_REGISTER_SIZE_INT },
        { 986, 2, TABLE_REGISTER_SIZE_LONG },
        { 989, 0, TABLE_REGISTER_SIZE_LONG },
        { 991, 0, TABLE_REGISTER_SIZE_INT },
        { 999, 0, TABLE_REGISTER_SIZE_INT },
        { 1007, 0, TABLE_REGISTER_SIZE_INT },
        { 1015, 0, TABLE_REGISTER_SIZE_INT },
        { 1016, 0, TABLE_REGISTER_SIZE_INT },
        { 1017, 0, TABLE_REGISTER_SIZE_INT },
        { 1018, 0, TABLE_REGISTER_SIZE_INT },
        { 1019, 0, TABLE_REGISTER_SIZE_INT },
        { 1020, 0, TABLE_REGISTER_SIZE_INT },
        { 1021, 0, TABLE_REGISTER_SIZE_INT },
        { 1022, 0, TABLE_REGISTER_SIZE_INT },
        { 1023, 0, TABLE_REGISTER_SIZE_INT },
        { 1024, 0, TABLE_REGISTER_SIZE_INT },
        { 1026, 0, TABLE_REGISTER_SIZE_INT },
        { 1027, 0, TABLE_REGISTER_SIZE_INT },

        { 1028, 2, TABLE_REGISTER_SIZE_INT },           // Component 1 (320)
        { 1029, 2, TABLE_REGISTER_SIZE_LONG },

        { 1036, 2, TABLE_REGISTER_SIZE_INT },
        { 1037, 2, TABLE_REGISTER_SIZE_LONG },

        { 1044, 2, TABLE_REGISTER_SIZE_INT },           // 10-05-2001 was 1046
        { 1045, 2, TABLE_REGISTER_SIZE_LONG },          // 10-05-2001 was 1047

        { 1052, 2, TABLE_REGISTER_SIZE_INT },
        { 1053, 2, TABLE_REGISTER_SIZE_LONG },

        { 1060, 2, TABLE_REGISTER_SIZE_INT },
        { 1061, 2, TABLE_REGISTER_SIZE_LONG },

        { 1068, 2, TABLE_REGISTER_SIZE_INT },
        { 1069, 2, TABLE_REGISTER_SIZE_LONG },

        { 1076, 2, TABLE_REGISTER_SIZE_INT },
        { 1077, 2, TABLE_REGISTER_SIZE_LONG },

        { 1084, 2, TABLE_REGISTER_SIZE_INT },
        { 1085, 2, TABLE_REGISTER_SIZE_LONG },
        
        { 1200, 0, TABLE_REGISTER_SIZE_INT },           // Diagnostics (352)
        { 1201, 0, TABLE_REGISTER_SIZE_INT },
        { 1202, 0, TABLE_REGISTER_SIZE_INT },
        { 1203, 0, TABLE_REGISTER_SIZE_INT },
        { 1204, 0, TABLE_REGISTER_SIZE_INT },
        { 1205, 0, TABLE_REGISTER_SIZE_INT },
        { 1206, 0, TABLE_REGISTER_SIZE_INT },
        { 1207, 0, TABLE_REGISTER_SIZE_INT },
        { 1208, 0, TABLE_REGISTER_SIZE_LONG },

        { 1210, 0, TABLE_REGISTER_SIZE_LONG },
        { 1212, 0, TABLE_REGISTER_SIZE_LONG },
        { 1214, 0, TABLE_REGISTER_SIZE_LONG },
        { 1216, 0, TABLE_REGISTER_SIZE_LONG },
        { 1218, 0, TABLE_REGISTER_SIZE_LONG },
        { 1220, 0, TABLE_REGISTER_SIZE_LONG },
        { 1222, 0, TABLE_REGISTER_SIZE_LONG },
        { 1224, 0, TABLE_REGISTER_SIZE_LONG },
        { 1226, 0, TABLE_REGISTER_SIZE_LONG },
        { 1228, 0, TABLE_REGISTER_SIZE_LONG },
        { 1230, 0, TABLE_REGISTER_SIZE_LONG },
        { 1232, 0, TABLE_REGISTER_SIZE_LONG },
        { 1234, 0, TABLE_REGISTER_SIZE_LONG },
        { 1236, 0, TABLE_REGISTER_SIZE_LONG },
        { 1238, 0, TABLE_REGISTER_SIZE_LONG },
        { 1240, 0, TABLE_REGISTER_SIZE_LONG },
        };      //                                                                              //End (376)


// Loader Control 
const int arrnLoaderModbusTable[LOADER_READ_TABLE_SIZE][3] = {
        { 0, 0, TABLE_REGISTER_SIZE_INT },              // Start of Common data, not actually modbus data
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },

        { 0, 0, TABLE_REGISTER_SIZE_INT },              // Control
        { 1, 0, TABLE_REGISTER_SIZE_INT },

        { 2, 0, TABLE_REGISTER_SIZE_INT },              // Calibration
        { 3, 0, TABLE_REGISTER_SIZE_INT },

        { 5, 0, TABLE_REGISTER_SIZE_INT },              // Summary
        { 6, 0, TABLE_REGISTER_SIZE_INT },
        { 7, 0, TABLE_REGISTER_SIZE_INT },
        { 9, 0, TABLE_REGISTER_SIZE_INT } };


// LOW Blender
const int arrnLOWBlenderModbusTable[LOW_BLENDER_READ_SIZE][3] = {
        { 0, 0, TABLE_REGISTER_SIZE_INT },              // Start of Common data, not actually modbus data
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },

        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 1, 0, TABLE_REGISTER_SIZE_INT },
        { 2, 0, TABLE_REGISTER_SIZE_INT } };




// Width Control
const int arrnWidthControlModbusTable[WIDTH_CONTROL_READ_SIZE][3] = {
        { 0, 0, TABLE_REGISTER_SIZE_INT },              // Start of Common data, not actually modbus data
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },

        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 1, 0, TABLE_REGISTER_SIZE_INT },
        { 2, 0, TABLE_REGISTER_SIZE_INT } };




// Profiler Control
const int arrnProfilerModbusTable[PROFILER_READ_SIZE][3] = {
        { 0, 0, TABLE_REGISTER_SIZE_INT },              // Start of Common data, not actually modbus data
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 0, 0, TABLE_REGISTER_SIZE_INT },

        { 0, 0, TABLE_REGISTER_SIZE_INT },
        { 1, 0, TABLE_REGISTER_SIZE_INT },
        { 2, 0, TABLE_REGISTER_SIZE_INT } };



#endif  // __MODBUSTABLES_H__
