

#!/bin/bash
name="Siddhartha"
echo "= SINGLE QUOTES ="
Preserve literal content
single='Hello $name'
echo "$single"
Ignore variable expansion
single2='The value of $name is not expanded'
echo "$single2"
Store quoted strings
quoted='Welcome to "Linux Shell"'
echo "$quoted"
Test edge cases
empty=''
echo "Empty string: [$empty]"
special='Special characters: $ @ # !'
echo "$special"
echo
echo "= DOUBLE QUOTES ="
Preserve spaces
message="Welcome to Linux Shell Scripting"
echo "$message"
Allow variable expansion
double="Hello $name"
echo "$double"
Parse nested tokens
nested="This is a 'nested' quote"
echo "$nested"
nested2="Name: $name, Welcome to 'Linux'"
echo "$nested2"
Validate outputs
echo "Name is: [$name]"
Test quoted command
echo "This command is inside double quotes"