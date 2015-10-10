import subprocess

proc = subprocess.Popen(['touch', 'file1.txt', 'file2.txt'],
                        stdin=subprocess.PIPE,
                        stdout=subprocess.PIPE
                       )

input_data = b"Some input"
proc.stdin.write(input_data)
output_data = proc.stdout.read()