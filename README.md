# Operating Systems and System Programs
 2110313 Operating Systems and System Programs 2025 Course, Chulalongkorn University

## Multipass Commands

```bash
# Reinstall Multipass every time cause daemon error
brew reinstall multipass

# Access an Instance 'Ubuntu-OS'
multipass shell Ubuntu-OS

# Exit an Instance
ubuntu@Ubuntu-OS:~$ exit

# Stop an Instance
multipass stop Ubuntu-OS

# List Instances
multipass list
```

## Linux Commands

```bash
# List all files and directories
ls
ls -f # with hidden files

# File content
cat file.txt # Display file content
cat > file.txt # Create file and write content

# Add current directory to PATH
PATH=$PATH:.

# C language
gcc -o filename filename.c # Compile C file
```
