
/***************************************************************************
* 
* Filename: riscv_alu_constants.sv
*
* Author: Arjun Singh
* Class: <323, 002, Winter>
* Date: <January 19, 2023>
*
* Description:  This is constants file that gets rid of the magic numbers 
*               in out project and puts them together in one place
*
****************************************************************************/

    localparam[6:0] OPCODE_SB = 7'b1100011;
    localparam[6:0] OPCODE_I_LOAD = 7'b0000011;
    localparam[6:0] OPCODE_I_ADD = 7'b0010011;
    localparam[6:0] OPCODE_S =7'b0100011;
    localparam[6:0] OPCODE_STORE = 7'b0100011;
    localparam[6:0] OPCODE_BRANCH = 7'b1100011;
    localparam[6:0] OPCODE_U = 7'b0110111;
    localparam[6:0] OPCODE_JAL = 7'b1101111;
    localparam[6:0] OPCODE_JALR = 7'b1100111;

    localparam[2:0] OPCODE_MSB = 3'd6;
    localparam[2:0] REG_MSB = 3'd4;
    localparam[2:0] PC_NEXT = 3'd4;
    localparam[2:0] ALU_CTRL_MSB = 3'd3;
    localparam[4:0] DATA_MSB = 5'd31;
    localparam[2:0] BOTTOM = 3'd0;
    localparam[4:0] SIGN_EXTEND_ONE = 5'd20;
    localparam[4:0] SIGN_EXTEND_TWO = 5'd19;
    localparam[4:0] SIGN_EXTEND_THREE = 5'd12;
    localparam[4:0] IMMI_BOTTOM = 5'd20;
    localparam[4:0] IMMU_BOTTOM = 5'd12;
    
    localparam[4:0] IMMS_MID_LSB = 5'd25;
    localparam[4:0] IMMS_BOTTOM_MSB = 5'd11;
    localparam[4:0] IMMS_BOTTOM_LSB = 5'd7;
    localparam[4:0] IMMSB_TOP_LSB = 5'd7;
    localparam[4:0] IMMSB_MID_MSB = 5'd30;
    localparam[4:0] IMMSB_MID_LSB = 5'd25;
    localparam[4:0] IMMSB_BOTTOM_MSB = 5'd11;
    localparam[4:0] IMMSB_BOTTOM_LSB = 5'd8;
    
    localparam[4:0] REGISTER_ONE_MSB = 5'd19;
    localparam[4:0] REGISTER_ONE_LSB = 5'd15;
    localparam[4:0] REGISTER_TWO_MSB = 5'd24;
    localparam[4:0] REGISTER_TWO_LSB = 5'd20;
    localparam[4:0] REGISTER_WRITE_MSB = 5'd11;
    localparam[4:0] REGISTER_WRITE_LSB = 5'd7; 

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
 
    localparam[2:0] FUNCT3_MSB = 5'd2; 
    localparam[11:0] SHIFT_LEFT = 12'b00000000; 

    localparam[4:0] JAL_MSB = 5'd31; 
    localparam[4:0] JAL_MIDDLE_MSB = 5'd19; 
    localparam[4:0] JAL_MIDDLE_BOTTOM = 5'd12; 
    localparam[4:0] JAL_MIDDLE = 5'd20; 
    localparam[4:0] JAL_LOWER_MSB = 5'd30; 
    localparam[4:0] JAL_LOWER_BOTTOM = 5'd21; 

    localparam[4:0] FUNCT3_INSTRUCTION_MSB = 5'd14; 
    localparam[4:0] FUNCT3_INSTRUCTION_LSB = 5'd12; 

    localparam[1:0] NEGATIVE = 1'b1;

    localparam[2:0] BEQ = 3'b000;
    localparam[2:0] BNE = 3'b001;
    localparam[2:0] BLT = 3'b100;
    localparam[2:0] BGE = 3'b101;