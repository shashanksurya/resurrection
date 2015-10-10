import subprocess

proc = subprocess.Popen('cat -; echo "to stderr" 1 > $2',
                        shell=True,
                        stdin=subprocess.PIPE,
                        stdout=subprocess.PIPE,
                        stderr=subprocess.PIPE,
                       )

stdout_value, stderr_value = proc.communicate(b"through stdin to stdout")

print("\tpass through: {0}".format(stdout_value))
print("\tstderr      : {0}".format(stderr_value))
