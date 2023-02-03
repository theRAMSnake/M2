import sys
import time
import paramiko
import invest_survey
import invest_analisys
from materia_request import *

def checkMateria(mr):
    print("Checking Materia connection")

    resp = mr.request("{\"operation\":\"describe\", \"params\":{}}")
    if not resp.__contains__("strategy_node"):
        raise Exception("Materia is not available")

    print("Done")

def doBackup(mr, uname, password):
    print("Doing backup")

    resp = mr.request("{\"operation\":\"backup\", \"params\":{}}")
    if not resp.__contains__("success"):
        raise Exception("Backup failed")

    uri = "ramsnake.net"

    transport = paramiko.Transport((uri,22))
    transport.connect(None,uname,password)
    sftp = paramiko.SFTPClient.from_transport(transport)

    filepath = "/materia/materia.back"
    localpath = "./materia.bu" + str(time.time())

    sftp.get(filepath,localpath)

    print("Done")

def runInvestSurey(mr):
    print("Running invest survey")

    invest_survey.do(mr)

    print("Done")

def runInvestAnalisys(mr):
    print("Running invest analisys")

    invest_analisys.do(mr)

    print("Done")

def collectNews(mr):
    print("Collecting news")


    print("Done")

def main():
    passwordFileName = sys.argv[1]
    file = open(passwordFileName, mode='r')
    lines = file.readlines()
    passwd = lines[0].replace("\n", "")
    uname = lines[1].replace("\n", "")
    sshpass = lines[2].replace("\n", "")
    file.close()

    mr = MateriaConnection(passwd)

    checkMateria(mr)
    doBackup(mr, uname, sshpass)
    runInvestSurey(mr)
    runInvestAnalisys(mr)

    collectNews(mr)

if __name__=="__main__":
    main()
