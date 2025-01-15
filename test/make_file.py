def makefile(filename,numlen):
    s = ""
    for i in range(numlen):
        s+=f"{i}\n"
    with open(filename,'w') as f:
        f.write(s)


if __name__ == "__main__":
    makefile("ymodem-test.txt",10000)