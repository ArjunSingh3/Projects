
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

  /*
    * 0000 -> AND
    * 0001 -> OR
    * 0010 -> Addition
    * 0110 -> Subtraction
    * 0111 -> Less Than
    * 1000 -> Shift Right Logic
    * 1001 -> Shift Left Logic
    * 1010 -> Shift Right Arithmetic
    * 1101 -> XOR
    */

    
    localparam[3:0] ALUOP_AND = 4'b0000;
    localparam[3:0] ALUOP_OR = 4'b0001;
    localparam[3:0] ALUOP_ADD = 4'b0010;
    localparam[3:0] ALUOP_SUB = 4'b0110;
    localparam[3:0] ALUOP_LESS_THAN = 4'b0111;
    localparam[3:0] ALUOP_SRL = 4'b1000;
    localparam[3:0] ALUOP_SLL = 4'b1001;
    localparam[3:0] ALUOP_SRA = 4'b1010;
    localparam[3:0] ALUOP_XOR = 4'b1101;
    
    
    localparam[2:0]  INPUT_ADD= 3'b000;
    localparam[2:0]  INPUT_SUB= 3'b001;
    localparam[2:0]  INPUT_AND= 3'b010;
    localparam[2:0]  INPUT_OR= 3'b011;
    localparam[2:0]  INPUT_XOR= 3'b100;
    localparam[2:0]  INPUT_LESS_THAN= 3'b101;
    localparam[2:0]  INPUT_SLL= 3'b110;
    localparam[2:0]  INPUT_SRA= 3'b111;
    