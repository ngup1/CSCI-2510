#!/bin/bash
# * Assume we have solution executable and student's slush.c already in directory
# * Command to run : bash autograde.sh
# * Command to run with verbosity: bash autograde.sh -v

# Initialize verbose flag. Default: verbosity off
verbose=0

# Convenient function to print messages in red
print_red () {
	red="\e[91m"
	white="\e[0m"

	printf "$red$1$white"
}

# Convenient funnction to print messages in green
print_green () {
	green="\e[92m"
	white="\e[0m"

	printf "$green$1$white"
}

# Convenient funnction to print messages in yellow
print_yellow () {
	yellow="\e[93m"
	white="\e[0m"

	printf "$yellow$1$white"
}

# Text after this function will print in red
start_red () {
	printf "\e[91m"
}

# Text after this function will print in white
end_red () {
	printf "\e[0m"
}


# Function to echo messages if verbose is enabled
print_verbose() {
  if [ $verbose -eq 1 ]; then
    printf "$@"
  fi
}

# Function to run a single command 
#	parameter 1: command to run via student's slush
#	parameter 2: command to run via terminal
#	parameter 3: test description
run_test() {
	local test_description=$1
	local command_student=$2
	local command_solution=$3

	student_file="output_student.txt"
	solution_file="output_solution.txt"

	print_verbose "GRADER: Creating File: $solution_file.\n"
	eval "$command_solution > \"$solution_file\""

	print_verbose "GRADER: Creating File: $student_file.\n"
	echo $command_student | ./slush > "$student_file"

	# Check if the student's output contains the solution's output
	if grep -qFx -f "$solution_file" "$student_file"; then
		print_green "GRADER: TEST PASSED - Case $test_description\n"
	else
		start_red
		printf "GRADER: TEST FAILED - Case $test_description\n"
		printf "GRADER: Solution output:\n\n"
		cat $solution_file
		printf "\nGRADER: Your output:\n\n"
		cat $student_file
		printf "\nGRADER: Your output must include the solutions output to pass autograde process.\n"
		end_red
        exit -1
	fi
	
	clean_up $student_file
	clean_up $solution_file
}


#####################################
# Cleanup function: deleting files given name
# 	First parameter:  file name
clean_up() {
  local file_name=$1
  rm -f "$file_name"
  print_verbose "GRADER: Deleting File: $file_name\n"
}

# Function to compile a program
# 	First parameter:  input file name to compile
# 	Second parameter: output file name
compile_program() {
    local input_file=$1
    local output_file=$2

    print_verbose "GRADER: Compiling program $input_file ...\n"
    gcc "$input_file" -o "$output_file"

    local result=$?  # Capture the exit status of the last command (gcc)
    if [[ $result == "0" ]]; then
        print_green "GRADER: Compiler reports no errors\n"
    else
        print_red "GRADER: Compiler error, QUITTING EARLY\n"
        exit -1
    fi
}

#!/bin/bash

# Define usage function
usage() {
    echo "USAGE: $0 [-v] [-q question_number]"
    echo "Options:"
    echo "  -v: OPTIONAL enables verbose mode." 
    echo "  -q q<question_number>: OPTIONAL specifies the question number to run. If none, it will conduct all test cases"
    exit -1
}

#####################################
# MAIN: 

# Variables to hold options
verbose=0
question_number=""

# Parse command-line options
while getopts "vq:" opt; do
    case ${opt} in
        v )
            verbose=1
            ;;
        q )
            question_number=$OPTARG
            ;;
        \? )
            usage
            ;;
        : )
            usage
            ;;
    esac
done

# Enable verbosity if requested
if [ $verbose -eq 1 ]; then
    print_yellow "GRADER: verbosity enabled\n"
fi

compile_program slush.c slush

# Perform actions based on the provided question number
# Check if question number is provided
if [ -z "$question_number" ]; then
    print_yellow "Running All Test Cases 1 through 4\n"
	run_test "1: Single command with arguments" "ls" "ls" 
	run_test "2: Single command with arguments" "ls -l" "ls -l" 
	run_test "3: two commands piped" "grep slush ( ls -l" "ls -l | grep slush"
	run_test "4: three commands piped" "sort ( grep slush ( ls -l" "ls -l | grep slush | sort" 
else
case $question_number in
    q1 )
        # Execute actions for question 1
        print_yellow "Running Test Case 1\n"
        run_test "1: Single command with arguments" "ls" "ls" 
        ;;
    q2 )
        # Execute actions for question 2
        print_yellow "Running Test Case 2\n"
        run_test "2: Single command with arguments" "ls -l" "ls -l" 
        ;;
    q3 )
        # Execute actions for question 2
        print_yellow "Running Test Case 3\n"
        run_test "3: two commands piped" "grep slush ( ls -l" "ls -l | grep slush"
        ;;
    q4 )
        # Execute actions for question 2
        print_yellow "Running Test Case 4\n"
        run_test "4: three commands piped" "sort ( grep slush ( ls -l" "ls -l | grep slush | sort" 
        ;;
    * )
        print_red "GRADER: ERROR - Invalid question number $question_number\n\n"
        usage
        ;;
esac
fi


# UNTESTED
## Case 5: Change directory
# ! unsure how to create tests for cd tmp; pwd
#### mkdir tmp && echo "cd tmp" | ./slush && echo "pwd" | ./slush

## Case 6: Non-existent command
#### echo "fakecommand" | ./slush

## Case 7: Invalid piping syntax
#### echo "ls -l ( " | ./slush

## Case 8: Signal Handling - SIGINT during a long-running command
# Action: Run ./slush manually, type a long-running command like find /, and then press ^C.

## Case 9: Exit on EOF
# echo "^D" | ./slush
# Expected Outcome: SLUSH exits upon receiving EOF character.

## Case 10: Exceeding argument limits
# Command: Construct a command with more than 15 arguments and pipe it into SLUSH.