# HarrAi

HarrAi is a smart C++ terminal assistant for Linux. It can run real shell commands and also understand simple natural-language requests like:

- `ls` or `list files`
- `pwd` or `show current directory`
- `ps` or `show processes`
- `df` or `show disk usage`
- `env` or `show environment`
- `create folder demo`
- `remove folder olddir`
- `show file /etc/os-release`
- `install package nginx`

## How to download and run it

### 1) Install the needed tools
On Ubuntu or Debian Linux, run:

```bash
sudo apt update
sudo apt install g++ make
```

### 2) Download or open the project
If you are using Git:

```bash
git clone https://github.com/dangquochai6a7thcsnt-hue/HarrAI.git
cd HarrAI
```

If the files are already in your workspace, just go into the folder:

```bash
cd /workspaces/HarrAI
```

If that path does not exist on your machine, run `pwd` or `ls /workspaces` to find the correct folder name.

### 3) Build the program

```bash
make
```

### 4) Run the assistant

```bash
./harrai
```

## Example commands

Inside the app you can try:

```text
help
ls
pwd
ps
df
whoami
create folder demo
show current directory
install package nginx
exit
```

## Tips

- You can type regular Linux commands directly.
- HarrAi also understands simple phrases, so `show current directory` works the same as `pwd`.
- If you want to change directories, use `cd /path/to/folder`.

## Make it smarter

You can keep improving HarrAi by adding more natural-language phrases, aliases, or custom shortcuts in the C++ source file.
