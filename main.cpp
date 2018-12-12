//
//  main.cpp
//  co_project
//
//  Created by 宋初文 on 12/9/18.
//  Copyright © 2018 宋初文. All rights reserved.
//

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <map>
using namespace std;


/*
 * It reads in instructions from the input file
 * If it successefully reads in the instructions, then return true
 * Else return false
 */
bool getInstructions(const char argc[], vector<string> &instructions, map<string, int> & branches){
    ifstream myfile;
    myfile.open(argc);
	
	// If cannot open input file, then return false
	if (!myfile) return false;
	
	string instruction;
	int line_num = 0;
	while (getline(myfile, instruction)) {
		line_num++;
		instructions.push_back(instruction);
		// Check if this is a branch label by checking the last char
		int len = instruction.length();
		if (instruction[len - 1] == ':') {
			// Get the branch name
			string branch_name = instruction.substr(0, len - 1);
			// The branch starts from the next line
			branches[branch_name] = line_num;
		}
	}
    myfile.close();
    return true;
}

void set_cycleStages(vector<vector<string> > &cycle_stages, int i, int j, int current_stage){
	if (current_stage == 1) {
		for (int k = 0; k < j; k++) {
			cycle_stages[k].push_back(".");
		}
		cycle_stages[j].push_back("IF");
		for (int k = j + 1; k < 16; k++) {
			cycle_stages[k].push_back(".");
		}
	}
	else if (current_stage == 2) {
		cycle_stages[j][i] = "ID";
	}
	else if (current_stage == 3) {
		cycle_stages[j][i] = "EX";
	}
	else if (current_stage == 4) {
		cycle_stages[j][i] = "MEM";
	}
	else if (current_stage == 5) {
		cycle_stages[j][i] = "WB";
	}
	
	return;
}

/*
 * If this instruction is a branch name, then return "branch"
 * Else if the operation is J type, then return "J"
 * Else if the operation is I type, then return "I"
 */
std::string getOperation(string instruction, string &operation){
	// Get the length of this instruction
	int len = instruction.length();

	// If this is a branch label, then no operation and return "branch"
	if (instruction[len - 1] == ':') {
		return "branch";
	}

	int index = 0;
	// Get the length of operation
	while (instruction[index] != ' ') index++;

	operation = instruction.substr(0, index);

	if (operation == "bne" || operation == "beq") {
		return "J";
	}
	else {
		return "I";
	}
}

/*
 * N: not J type, get destination and operands
 * J: J type, get operands, destination is the name of branch
 */
void read_instruction(const string instruction, string &destination, string &operand1, string &operand2, char type) {
	// If not J type
	if (type == 'N') {
		int operand_count = 0;
		for (int i = 0; i < instruction.length(); i++) {
			if (instruction[i] == '$') {
				// If operand_count is 0, then it is the destination
				if (operand_count == 0) {
					operand_count++;
					destination = instruction.substr(i + 1, 2);
				}
				// Else is operand2
				else if (operand_count == 1) {
					operand2 = instruction.substr(i + 1, 2);
					operand_count++;
				}
			} 
			else if (instruction[i] == ',' && operand_count == 2) {
				// If the second operand is a register
				if (instruction[i + 1] == '$') {
					operand2 = instruction.substr(i + 2, 2);
				}
				// Else if the second operans is a number
				else {
					operand2 = instruction.substr(i + 1, instruction.length() - i - 1);
				}
			}
		}
	}
	// If J type
	else if (type == 'J') {
		int operand_count = 0;
		for (int i = 0; i < instruction.length(); i++) {
			if (instruction[i] == '$') {
				if (operand_count == 0) {
					operand_count++;
					operand1 = instruction.substr(i + 1, 2);
				}
				else if (operand_count == 1) {
					operand2 = instruction.substr(i + 1, 2);
				}
			}
			// If operand_count is 1, that means we passed operand2 already
			if (operand_count == 1 && instruction[i - 1] == ',') {
				// Get the length of the name of the branch
				int len = instruction.length() - i;
				destination = instruction.substr(i, len);
			}
		}
	}
}

void set_destinationStage(string destination, int current_stage, map<string, int> &destinations) {
	destinations[destination] = current_stage;
	return;
}

/*
 * c: updata b value
 * u: update a value
 */
void update_registerFile(){

}

void get_value(){
    
}

void get_branchIndex(){
    
}

void print_cycle(){
    
}

int main(int argc, const char * argv[]) {
    if (argc != 3){
		cerr << "Invalid usage" << endl;
        return -1;
    }
    
    std::vector<std::string> instructions;
    std::map<std::string, int> branches;
    std::map<std::string, int> destinations;
    std::map<std::string,std::pair<int, int> > register_file;
    std::vector<std::string> cycle_instructions;
    std::vector<std::vector<std::string> > cycle_stages;
    
    
    // Read instructions from input file into the vector 
    // If it fails in reading, then return 1 to terminate the program
    if (!getInstructions(argv[2], instructions, branches)) {
		cerr << "Cannot open input file" << endl;
        return -1;
    }

	cycle_instructions.push_back(instructions[0]);

    for (int i = 0; i < 16; i++){
        int next_instruction_index = 1;
        for (int j = 0; j <= i; j++){
            //get current_stage
            int current_stage;
            std::string current_instruction = instructions[j];
            if (i-j >= 5){
                current_stage = 5;
            }
            else{
                current_stage = i-j;
            }
            set_cycleStages(cycle_stages, i, j, current_stage);
            
            std::string operation;
            std::string instruction_type;
            // Get operation and its type
            instruction_type = getOperation(current_instruction, operation);

            std::string destination;
            std::string operand1;
            std::string operand2;

            // If the operation is a branch label, then skip to the next instruction
            if (operation=="branch"){
                cycle_instructions[j] = instructions[next_instruction_index];
                next_instruction_index++;
                j--;
                continue;
            }
            if (instruction_type != "J"){
                //get destination, operands
                read_instruction(current_instruction, destination, operand1, operand2, 'N');
                
                //update destination stage
                set_destinationStage(destination, current_stage, destinations);
                if(current_stage == 3){
                    //update_registerFile(operation, destination, operand1, operand2, register_file, c);
                }
                
            }
            else if (instruction_type=="J"){
                read_instruction(current_instruction, destination, operand1, operand2, 'J');
                if(current_stage==5){
                    //update next three instructions
                    for (int k = 1; k < 4; k++){
                        cycle_instructions[j+k] = instructions[next_instruction_index+k-1];
                        //get destination, operands(local)
                        //update destination stage
                        //set_cycleStages
                        //if stage=3, update_registerFile
                    }
                    int operand1_value;
                    int operand2_value;
                    //get_value(operand1, register_file, operand1_value);
                    //get_value(operand2, register_file, operand2_value);
                    
                    //compare operands, decide whether to take the branch
                    if ((operation=="beq" && operand1_value==operand2_value) ||(operation=="bne" && operand1_value!=operand2_value)){
                        //get_branchIndex(destination, destinations, next_instruction_index);
                        cycle_instructions[j+4] = instructions[next_instruction_index];
                        //set_cycleStages with stall
                        next_instruction_index++;
                    }
                    else{
                        cycle_instructions[j+4] = instructions[next_instruction_index];
                        //set_cycleStages without stall
                        next_instruction_index++;
                    }
                    j=j+4;
                }
            }
            cycle_instructions[j+1] = instructions[next_instruction_index];//这里的index我可能算错了
            next_instruction_index++;
            if(current_stage==5){
                //update_registerFile(a);
            }

            
        }
        //print cycle_instructions, cycle_stages
        //print_cycle(cycle_instructions, cycle_stages, i);
    }
    
    return 0;
}
