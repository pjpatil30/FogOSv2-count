FogOS - Count Implementation

General Overview:
count command is similar to grep, as it looks through files for a specific word,
and lists the number of times the word appeared within each file, and provides a
total count at the end.

Step 1: Build
- Build the OS using make qemu to be inside the kernel.

Step 2:
- Inside the OS, simply choose one of the options stated below:
	- count [word]
		- This search will look for the given word within the current directory (".") and
		will print out the total count.
		- Output: 
			./file1: 1
			./file2: 4
			./file3: 0
			./file4: 0
			Total Count: 5

	- count [word] [file]
		- This search will look for the given word within the stated file and print out the
		total count. *Note* if the file does not exist, function will throw error and quit.
		- Output:
			file: 3
			Total Count: 3

	- count [word] [file1] [file2] ...
		- This search will look for the given word within the multiple stated files and will
		print out the total count.
		- Output:
			file1: 1
			file2: 5
			Total Count: 6

Testing:
	- test1.txt
		- has the word test once
	- test2.txt
		- has the word count twice
		- has the word test 3 times

Test Results;
$ count count test1.txt test2.txt
test1.txt : 0
test2.txt : 2
Total Count: 2

$ count test test1.txt test2.txt
test1.txt : 1
test2.txt : 3
Total Count: 4

$ count test
./README.md : 0
./test1.txt : 1
./test2.txt : 3
./cat : 0
./count : 0
./echo : 0
./forktest : 0
./grep : 0
./init : 0
./kill : 0
./ln : 0
./ls : 0
./mkdir : 0
./rm : 0
./sh : 0
./stressfs : 0
./usertests : 0
./grind : 0
./wc : 0
./zombie : 0
./logstress : 0
./forphan : 0
./dorphan : 0
Total Count: 4
