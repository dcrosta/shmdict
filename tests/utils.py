import subprocess

class IPCS(object):
    """Interact with the `ipcs` command (assumes a *NIX)
    """

    @classmethod
    def check_output(cls, *shell_command):
        proc = subprocess.Popen(shell_command, stdout=subprocess.PIPE, shell=True)
        stdout, _ = proc.communicate()
        if proc.returncode != 0:
            raise subprocess.CalledProcessError(proc.returncode, shell_command, stdout)
        return stdout

    @classmethod
    def list(cls):
        output = (line for line in cls.check_output("ipcs").splitlines())

        for line in output:
            if line.startswith("Shared Memory:"):
                break

        segments = []
        for line in output:
            if line == "":
                break
            segments.append(dict(zip(
                ("id", "key", "mode", "owner", "group"),
                [part for part in line.split() if part][1:])))

        return segments


