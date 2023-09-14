
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 02/22/2023 08:15:38 PM
// Design Name: 
// Module Name: riscv_multicycle_constants
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


    localparam[6:0] FUNCT7_ADD_SRL = 7'b0000000;
    localparam[6:0] FUNCT7_SUB = 7'b0100000;
    localparam[6:0] OPCODE_R = 7'b0110011;
    localparam[2:0] FUNCT3_ADD_SUB = 3'b000;
    localparam[2:0] FUNCT3_SLL = 3'b001;
    localparam[2:0] FUNCT3_SLT = 3'b010;
    localparam[2:0] FUNCT3_SLTI = 3'b011;
    localparam[2:0] FUNCT3_XOR = 3'b100;
    localparam[2:0] FUNCT3_SRL_SRA = 3'b101;
    localparam[2:0] FUNCT3_OR = 3'b110;
    localparam[2:0] FUNCT3_AND = 3'b111;
    
