`timescale 1ns / 1ps
/***************************************************************************
* 
* Filename: riscv_final.sv
*
* Module: <riscv_final>
*
* Author: <Arjun Singh>
* Class: <323, 002, Winter>
* Date: <March 18, 2023>
*
* Description: <THis processor uses pipelining with forwarding. It also consists of hazard detection and forwarding units for branch and load use stall>
*
****************************************************************************/

module riscv_final #(parameter[31:0] INITIAL_PC = 32'h00400000)(clk,rst,PC, instruction, 
                                                                              ALUResult, dAddress, 
                                                                              dWriteData, dReadData, 
                                                                              MemRead, MemWrite, 
                                                                              iMemRead,WriteBackData);
    
    `include "riscv_datapath_constants.sv"
    `include "riscv_alu_constants.sv"
    // 1 Bit I/O wires
    input wire logic clk, rst;
    output logic MemRead, MemWrite,iMemRead;
    // 32 Bit I/O wires
    input wire logic [DATA_MSB:BOTTOM] instruction, dReadData;
    output logic[DATA_MSB:BOTTOM] PC, ALUResult, dAddress, dWriteData, WriteBackData;
    ///////////////////////////////////////////////
    //////////MEM STAGE///////////////////////////
        // Internal Logic Wires and signals:
        logic [DATA_MSB:BOTTOM] mem_readData1, mem_writeData;
        logic mem_MemWrite, mem_MemRead, mem_Branch, mem_RegWrite, mem_MemToReg, mem_zero, mem_less_than;
        logic [REG_MSB:BOTTOM] mem_writeReg;
        logic [DATA_MSB:BOTTOM] mem_result;
        logic [FUNCT3_MSB:BOTTOM] mem_funct3;
        logic mem_jal, mem_jalr;
        logic [DATA_MSB:BOTTOM] mem_new_PC;

        ///////////////////////////////////////////////
    
    /////////////////////////////////////////////////////////////////////////////////////
    // Forwarding Unit
    ////////////////////////////////////////////////////////////////////////////////////
    // Forward the Following rs1,rs2 regsiters:
    logic [REG_MSB:BOTTOM]ex_readReg1, ex_readReg2;
    logic jal_precedence;
    logic mem_jal_r_jump, wb_jal_r_jump;

    ///////////////////////////////////////////////////////////////////////////////////
    // Load_use_hazard - Detects the presence of the 'load-use' hazard in the pipeline
    logic Load_use_hazard;
    // branch_mem_taken - Detects that a branch is in the MEM stage and it is taken
    logic branch_mem_taken;
    // branch_wb_taken- Detects that a branch is in the WB stage and it is taken
    logic branch_wb_taken;
    // branch_precedence - Checks for RBanch_load_use_hazard, and only goes through with the branch hazar
    logic branch_precedence;
    
    ///////////////////////////////////////////////
        // Internal Logic Wires and signals:
      
        logic[DATA_MSB:BOTTOM]  wb_writeData, wb_result;
        logic wb_MemToReg,wb_RegWrite, mem_PCSrc, wb_PCSrc;
        logic[REG_MSB:BOTTOM] wb_writeReg;
        logic [DATA_MSB:BOTTOM] mem_PC;
        ///////////////////////////////////////////////
    
    ////////////////////////////////////////////
    // IF: Instruction Fetch

        ///////////////////////////////////////////////
        // Internal Logic Wires and signals:
        logic [DATA_MSB:BOTTOM] if_PC;
        logic [DATA_MSB:BOTTOM] mem_imm;

        ///////////////////////////////////////////////
        /*
        * This register updates the PC every clock cycle for the next or the branched instruction value
        */
        always_ff @(posedge clk) begin
            if(rst) begin
                if_PC <= INITIAL_PC;
            end
            // Stalls the pipeline by not updating the PC during a loaduse stall only and gives precedence to branch taken instructions
            // ******************************************************************************//
            else if(Load_use_hazard && !(branch_precedence|| jal_precedence)) begin
                if_PC<=if_PC;
            end
            // *****************************************************************************//
            // ASK ABOUT THIS: LOADPC AND MEMPCSRC SIGNAL
            else if (mem_PCSrc) begin
                if(mem_Branch | mem_jal) begin
                    if_PC <= mem_PC + mem_imm;
                end
                else begin
                    if_PC <= mem_new_PC;
                end 
            end
            else begin
                //////////// MAGIC NUMBER 4///REMOVE IT//////////
                if_PC <= if_PC + PC_NEXT;
            end     
        end

        logic [DATA_MSB:BOTTOM] id_instruction;
        
        assign id_instruction = instruction;
   
    // TOP LEVEL PORT PC
    assign PC = if_PC;

    assign iMemRead = (!Load_use_hazard || (branch_precedence||jal_precedence));
    ///////////////////////////////////////////
    ///////////////////////////////////////////
    // ID: Instruction Decoder

        ///////////////////////////////////////////////
        // Internal Logic Wires and signals:
        logic [DATA_MSB:BOTTOM] id_PC, id_readData1, id_readData2, id_writeData;
        logic id_ALUSrc, id_MemWrite, id_MemRead, id_Branch, sid_RegWrite, id_MemToReg, id_PCSrc;
        logic [ALU_CTRL_MSB:BOTTOM] id_ALUCtrl;
        logic [REG_MSB:BOTTOM]id_readReg1, id_readReg2,id_writeReg;
        logic [OPCODE_MSB:BOTTOM] opcode;
        logic [DATA_MSB:BOTTOM] id_imm, immI, immS, immSB,immU,imm_JAL;
        logic stall,U_type, id_jal,id_jalr;

        ///////////////////////////////////////////////
        
        // Pipeline Register IF/ID: pipeline the PC
        always_ff @(posedge clk) begin
            if(rst) begin
                id_PC <= INITIAL_PC;
                //id_instruction <= 0;
            end
            else if(Load_use_hazard && !branch_precedence) begin
                id_PC<=id_PC;
            end
            else begin
                id_PC <= if_PC;
                //id_instruction <= if_instruction;
            end
        end

        
        // 1. Decode the instruction and generate the control signals:
            // OPCODE
        assign opcode = id_instruction[OPCODE_MSB:BOTTOM];
        assign id_Branch = (id_instruction[OPCODE_MSB:BOTTOM] == OPCODE_BRANCH)? 1:0;
            // Calculate the ALUCtrl 4 bits long
        always_comb begin
            if(id_instruction[OPCODE_MSB:BOTTOM] == OPCODE_I_LOAD || id_instruction[OPCODE_MSB:BOTTOM] == OPCODE_STORE || id_instruction[OPCODE_MSB:BOTTOM] == OPCODE_U ) begin
                id_ALUCtrl = ALUOP_ADD;
            end
            else if(id_instruction[OPCODE_MSB:BOTTOM] == OPCODE_BRANCH ) begin
                id_ALUCtrl = ALUOP_SUB;
            end
            else begin
                // Switch statement to choose the R-Type
                case (id_instruction[FUNCT3_INSTRUCTION_MSB:FUNCT3_INSTRUCTION_LSB])
                    FUNCT3_ADD_SUB: 
                        if(id_instruction[DATA_MSB:IMMSB_MID_LSB] == FUNCT7_ADD_SRL) begin
                            id_ALUCtrl = ALUOP_ADD;
                        end
                        else if((id_instruction[DATA_MSB:IMMSB_MID_LSB] == FUNCT7_SUB) && (id_instruction[OPCODE_MSB:BOTTOM]==OPCODE_R)) begin
                            id_ALUCtrl = ALUOP_SUB;
                        end
                        else begin
                            id_ALUCtrl = ALUOP_ADD;
                        end
                    FUNCT3_SLL:
                        id_ALUCtrl = ALUOP_SLL;
                    FUNCT3_SLT:
                        id_ALUCtrl = ALUOP_LESS_THAN;
                    FUNCT3_SLTI: 
                        id_ALUCtrl = ALUOP_LESS_THAN;
                    FUNCT3_XOR:
                        id_ALUCtrl = ALUOP_XOR;
                    FUNCT3_SRL_SRA:
                        if(instruction[DATA_MSB:IMMSB_MID_LSB] == FUNCT7_ADD_SRL) begin
                            id_ALUCtrl = ALUOP_SRL;
                        end
                        else begin
                            id_ALUCtrl = ALUOP_SRA;
                        end
                    FUNCT3_OR:
                        id_ALUCtrl = ALUOP_OR;
                    FUNCT3_AND:
                        id_ALUCtrl = ALUOP_AND;
                   default:
                        id_ALUCtrl = ALUOP_ADD;
                endcase
            end
        end

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////// IMMIDIATE GENERATION BLOCK /////////////////////////////////////////////////////////////////////////////////////
            // ALL THE IMMIDIATES ARE CALCULATED FOR I TYPE, S TYPE, AND SB TYPE, and IMMU_Type
        //******************// Check this one and make sure it is correct //***********//
        assign immU = $signed({{{SIGN_EXTEND_THREE{id_instruction[DATA_MSB]}},
                                    id_instruction[DATA_MSB:IMMU_BOTTOM],SHIFT_LEFT}});
        assign imm_JAL = $signed({{SIGN_EXTEND_THREE{id_instruction[DATA_MSB]}},
                                    id_instruction[JAL_MSB],
                                    id_instruction[JAL_MIDDLE_MSB:JAL_MIDDLE_BOTTOM],
                                    id_instruction[JAL_MIDDLE],
                                    id_instruction[JAL_LOWER_MSB:JAL_LOWER_BOTTOM],1'b0});
        assign immI = $signed({{{SIGN_EXTEND_TWO{id_instruction[DATA_MSB]}},
                                    id_instruction[DATA_MSB:IMMI_BOTTOM]}});
        assign immS = $signed({{SIGN_EXTEND_TWO{id_instruction[DATA_MSB]}},
                                    id_instruction[DATA_MSB:IMMS_MID_LSB],
                                    id_instruction[IMMS_BOTTOM_MSB:IMMS_BOTTOM_LSB]});
        assign immSB = $signed({{SIGN_EXTEND_ONE{id_instruction[DATA_MSB]}},
                                    id_instruction[IMMSB_TOP_LSB],
                                    id_instruction[IMMSB_MID_MSB:IMMSB_MID_LSB],
                                    id_instruction[IMMSB_BOTTOM_MSB:IMMSB_BOTTOM_LSB],1'b0});

                                    // 2. Imm Generation Unit: GENERATES AN IMMIDIATE VALUE DEPENDING ON THE OPCODE AND WHIHCH INSTRUCTION IT IS
        always_comb begin
            U_type = 0;
            if((id_instruction[OPCODE_MSB:BOTTOM])== OPCODE_I_ADD || id_instruction[OPCODE_MSB:BOTTOM] == OPCODE_I_LOAD || id_instruction[OPCODE_MSB:BOTTOM] == OPCODE_JALR) begin
                id_imm = immI;
            end
            else if(id_instruction[OPCODE_MSB:BOTTOM] == OPCODE_S) begin
                id_imm = immS;
            end
            else if(id_instruction[OPCODE_MSB:BOTTOM] == OPCODE_SB) begin
                id_imm = immSB;
            end
            else if(id_instruction[OPCODE_MSB:BOTTOM] == OPCODE_U) begin
                U_type = 1;
                id_imm = immU;
            end
            else if(id_instruction[OPCODE_MSB:BOTTOM] == OPCODE_JAL) begin
                id_imm = imm_JAL;
            end
            //else if begin
            //end
            else begin
                id_imm = 0;
            end
        end
        //////**********************************************************************************************************************************////////                         
            // ALUSrc
        assign id_ALUSrc =(id_instruction[OPCODE_MSB:BOTTOM] == OPCODE_I_ADD || 
                        id_instruction[OPCODE_MSB:BOTTOM] == OPCODE_I_LOAD ||
                        id_instruction[OPCODE_MSB:BOTTOM] == OPCODE_S || id_instruction[OPCODE_MSB:BOTTOM] == OPCODE_U);
            // readReg1
        assign id_readReg1 = id_instruction[REGISTER_ONE_MSB:REGISTER_ONE_LSB];
            // readReg2
        assign id_readReg2 = id_instruction[REGISTER_TWO_MSB:REGISTER_TWO_LSB];
            // writeReg
        assign id_writeReg = id_instruction[REGISTER_WRITE_MSB:REGISTER_WRITE_LSB];
            // MemWrite
        assign id_MemWrite = id_instruction[OPCODE_MSB:BOTTOM] == OPCODE_STORE;
            // MemRead 
        assign id_MemRead =  id_instruction[OPCODE_MSB:BOTTOM] == OPCODE_I_LOAD;
            // RegWrite the 1 and 0 signal
        assign id_RegWrite = ((id_instruction[OPCODE_MSB:BOTTOM] != OPCODE_STORE && (id_instruction[OPCODE_MSB:BOTTOM] != OPCODE_BRANCH))|| id_instruction[OPCODE_MSB:BOTTOM] == OPCODE_JAL || id_instruction[OPCODE_MSB:BOTTOM] ==OPCODE_JALR );
            // MemtoReg
        assign id_MemToReg = (id_instruction[OPCODE_MSB:BOTTOM] == OPCODE_I_LOAD );
            // PCSrc  
        assign id_PCSrc = 0;
        
        assign id_jal = (id_instruction[OPCODE_MSB:BOTTOM] == OPCODE_JAL);
        assign id_jalr = (id_instruction[OPCODE_MSB:BOTTOM] == OPCODE_JALR);
        // Calculating the funct3 bits for branch instructions:
        /**
        *
        * BEQ -> 000
        * BNE -> 001
        * BLT -> 100
        * BGE -> 101
        * 
        */
        logic [FUNCT3_MSB:BOTTOM] id_funct3;
        // CEhck the bits again!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        assign id_funct3 = id_instruction[FUNCT3_INSTRUCTION_MSB:FUNCT3_INSTRUCTION_LSB];
     

        // 3. read the registers from the register file 
            // INSTANTIATING REG FILE
        regfile REGFILE(
                        .clk(clk), 
                        .write(wb_RegWrite),
                        .readReg1(id_readReg1),
                        .readReg2(id_readReg2),
                        .writeReg(wb_writeReg),
                        .writeData(WriteBackData),
                        .readData1(id_readData1),
                        .readData2(id_readData2)
                    );                                                                                                                            

    
   
    ///////////////////////////////////////////
    // EX: Execution
        
        ///////////////////////////////////////////////
        // Internal Logic Wires and signals:
        logic [DATA_MSB:BOTTOM] ex_PC, operand2,operand3,operand1,ex_readData1, ex_readData2, ex_writeData;
        logic ex_ALUSrc, ex_MemWrite, ex_MemRead, ex_Branch, ex_RegWrite, ex_MemToReg, ex_PCSrc, ex_zero, ex_U_type,ex_less_than;
        logic [ALU_CTRL_MSB:BOTTOM] ex_ALUCtrl;
        logic [REG_MSB:BOTTOM] ex_writeReg;
        logic [DATA_MSB:BOTTOM] ex_imm, ex_result, ex_ALU_result;
        logic [FUNCT3_MSB:BOTTOM] ex_funct3;
        logic ex_jal, ex_jalr;
        logic [DATA_MSB:BOTTOM] ex_new_PC;
        ///////////////////////////////////////////////

        // PipeLine Register ID/EX:
        always_ff @(posedge clk) begin 
            if(rst || Load_use_hazard || branch_mem_taken || branch_wb_taken || mem_jal_r_jump||wb_jal_r_jump) begin
                ex_PC <= INITIAL_PC;
                // INstructions that are passed onto from the previous stage:
                
                ex_ALUSrc <= 0;
                ex_MemWrite <= 0;
                ex_MemRead <= 0;
                ex_Branch <= 0;
                ex_RegWrite<= 0;
                ex_MemToReg <= 0;
                ex_PCSrc<= 0;
                ex_ALUCtrl <= 0;
                ex_readReg1 <= 0;
                ex_readReg2 <= 0;
                ex_writeReg <= 0;
                ex_imm <= 0;
                ex_U_type<=0;
                ex_funct3 <=0;
                ex_jal <= 0;
                ex_jalr <= 0;
            end
            else begin
                ex_PC <= id_PC;
                ex_ALUSrc <= id_ALUSrc;
                ex_MemWrite <= id_MemWrite;
                ex_MemRead <= id_MemRead;
                ex_Branch <= id_Branch;
                ex_RegWrite<= id_RegWrite;
                ex_MemToReg <= id_MemToReg;
                ex_PCSrc<= id_PCSrc;
                ex_ALUCtrl <= id_ALUCtrl;
                ex_readReg1 <= id_readReg1;
                ex_readReg2 <= id_readReg2;
                ex_writeReg <= id_writeReg;
                ex_imm <= id_imm;
                ex_U_type<=U_type;
                ex_funct3 <= id_funct3;
                ex_jal <= id_jal;
                ex_jalr <= id_jalr;
            end
        end
        //////////////////////////////////////////////
        // 1. ALU Operations
        assign ex_readData1 = ex_U_type? 0 : id_readData1;
        assign ex_readData2 = id_readData2;
        
        //////////////////////////////////////////////////////
        // brnach less than signal
        assign ex_less_than = (ex_result[DATA_MSB]==NEGATIVE)?1:0;
        //////////////////////////////////////////////////////////////////////////////////////////////
        // Forwarding Logic
        logic forward2_mem,forward2_wb;
        assign forward2_mem = ((ex_readReg2 == mem_writeReg) && (mem_RegWrite) && (mem_writeReg!=0));
        assign forward2_wb = ((ex_readReg2 == wb_writeReg)&& (wb_RegWrite) && (wb_writeReg!=0));

        // Forwarding logic for readData2
        always_comb begin
            if((ex_readReg2 == mem_writeReg) && (mem_RegWrite) && (mem_writeReg!=0)) begin
                operand3 = mem_result;
            end
            else if((ex_readReg2 == wb_writeReg)&& (wb_RegWrite) && (wb_writeReg!=0)) begin
                operand3 = WriteBackData;
            end
            else begin
                operand3 = ex_readData2;
            end
        end

        assign ex_new_PC = operand1 + ex_imm;

        logic forward1_mem,forward1_wb;

        assign forward1_mem = ((ex_readReg1 == mem_writeReg) && (mem_RegWrite) && (mem_writeReg!=0));
        assign forward1_wb = ((ex_readReg1 == wb_writeReg)&& (wb_RegWrite) && (wb_writeReg!=0));

        // Combination bloack for forwarding logic for readData 1
        always_comb begin
            if((ex_readReg1 == mem_writeReg) && (mem_RegWrite) && (mem_writeReg != 0)) begin
                operand1 = mem_result;
            end
            else if((ex_readReg1 == wb_writeReg)&& (wb_RegWrite) && (wb_writeReg != 0)) begin
                operand1 = WriteBackData;
            end
            else if(ex_U_type) begin
                operand1 = 0;
            end
            else begin
                operand1 = ex_readData1;
            end
        end

        // Combination logic which gives precedence to immediate value
        always_comb begin
            if(ex_ALUSrc) begin
                operand2 = ex_imm;
            end
            else begin
                operand2 = operand3;
            end
        end

        ///////////////////////////////////////////////////////////////////////////////////////////

        //ALU instantiation
        alu ALU(.op1(operand1),.op2(operand2),.alu_op(ex_ALUCtrl),.result(ex_result),.zero(ex_zero));


        // TOP LEVEL PORT ALU_Result

        assign ex_ALU_result = (ex_jal || ex_jalr)? (ex_PC+PC_NEXT): ex_result ;
        assign ex_writeData = operand3;
        assign ALUResult = ex_result;
        // 2. Computing BRANCH TARGET ADDRESS
    ///////////////////////////////////////////
    ///////////////////////////////////////////
    // MEM: Memory Read or Write

        
        
        // PipeLine Register EX/MEM:
        always_ff @(posedge clk) begin 
            if(rst || branch_mem_taken || mem_jal_r_jump) begin
                mem_PC <= INITIAL_PC;
                // INstructions that are passed onto from the previous stage:
                mem_MemWrite <= 0;
                mem_MemRead <= 0;
                mem_Branch <= 0;
                mem_RegWrite<= 0;
                mem_MemToReg <= 0;
                //mem_PCSrc<= 0;
                mem_writeReg <= 0;
                mem_writeData <= 0;
                mem_imm <= 0;
                mem_result <=0;
                mem_zero <= 0;
                mem_funct3 <= 0;
                mem_less_than <= 0;
                mem_jal <= 0;
                mem_jalr <= 0;
                mem_new_PC <= 0;
            end
            else begin
                mem_PC <= ex_PC;
                mem_result <= ex_ALU_result;
                mem_MemWrite <= ex_MemWrite;
                mem_MemRead <= ex_MemRead;
                mem_Branch <= ex_Branch;
                mem_RegWrite<= ex_RegWrite;
                mem_MemToReg <= ex_MemToReg;
                //mem_PCSrc<= ex_PCSrc;
                mem_writeReg <= ex_writeReg;
                mem_writeData <= ex_writeData;
                mem_imm <= ex_imm;
                mem_zero <= ex_zero;
                mem_funct3 <= ex_funct3;
                mem_less_than <= ex_less_than; 
                mem_jal <= ex_jal;
                mem_jalr <= ex_jalr;
                mem_new_PC <= ex_new_PC;
            end
        end
        
        assign dAddress = mem_result;
        // This will be changed to handle many more branch instructions
        // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        // assign mem_PCSrc = (mem_Branch && mem_zero);
        //   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        // TOP LEEVEL PORT dWriteData
        assign dWriteData = mem_writeData;
        assign MemRead = mem_MemRead;
        assign MemWrite = mem_MemWrite;

         // Calculating the funct3 bits for branch instructions:
        /**
        *
        * BEQ -> 000
        * BNE -> 001
        * BLT -> 100
        * BGE -> 101
        * 
        */
        
        always_comb begin
            mem_PCSrc = 0;
            if(mem_jal || mem_jalr) begin
                mem_PCSrc = 1;
            end
            else if(mem_Branch) begin 
                case(mem_funct3)
                    BEQ: 
                        if (mem_zero) begin 
                            mem_PCSrc = 1;
                        end
                        else begin
                            mem_PCSrc = 0;
                        end
                    BNE:
                        if(!(mem_zero)) begin
                           mem_PCSrc = 1; 
                        end
                        else begin
                            mem_PCSrc = 0;
                        end
                    BLT:
                        if(mem_less_than)begin
                            mem_PCSrc =1;
                        end
                        else begin
                            mem_PCSrc = 0;
                        end
                    BGE:
                        if (!mem_less_than || mem_zero) begin
                            mem_PCSrc = 1;
                        end
                        else begin
                            mem_PCSrc = 0;
                        end
                    default:
                        mem_PCSrc = 0;
                endcase
            end
            else begin 
                mem_PCSrc = 0;
            end
        end
        
    ///////////////////////////////////////////
    ///////////////////////////////////////////
    // WB: Write Back
    ///////////////////////////////////////////////
        // Internal Logic Wires and signals:
        //logic [31:0] wb_PC;
        //logic[31:0] wb_writeReg, wb_writeData,wb_RegWrite;
        //logic wb_MemToReg;
        ///////////////////////////////////////////////
        // Mem/WB Pipeline Register
        always_ff@(posedge clk) begin
            if(rst) begin
                wb_writeReg <=0;
                wb_writeData <= 0;
                wb_RegWrite <=0;
                wb_MemToReg <=0;
                wb_result <= 0;
                wb_PCSrc<=0;
            end
            else begin
                wb_writeReg <= mem_writeReg;
                wb_writeData <= mem_writeData;
                wb_RegWrite <= mem_RegWrite;
                wb_MemToReg <= mem_MemToReg;
                wb_result <= mem_result;
                wb_PCSrc<=mem_PCSrc;
            end
        end
        
        // TOP LEVEL PORT WriteBackData:
        assign WriteBackData = wb_MemToReg? dReadData:wb_result;

    ///////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Hazard Detection Logic
    
    /////////////////////////////////////////////
    // Detects laod use hazard
    assign Load_use_hazard = ((ex_MemRead==1) &&((ex_writeReg==id_readReg1)||(ex_writeReg==id_readReg2)));
    // detects branch taken in the MEM stage
    assign branch_mem_taken = mem_PCSrc;
    assign mem_jal_r_jump = mem_PCSrc;
    // detects branch taken in the WB stage
    assign branch_wb_taken = wb_PCSrc;
    assign wb_jal_r_jump = wb_PCSrc;
    // detects branch precendece durin branch load use stall
    assign branch_precedence = (branch_mem_taken || branch_wb_taken);
    assign jal_precedence = (mem_jal_r_jump||wb_jal_r_jump);
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
endmodule


