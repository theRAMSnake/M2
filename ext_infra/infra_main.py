import sys
import time
import paramiko
import invest_survey
import invest_analisys
import make_news
import requests
import json
from materia_request import *

def checkMateria(mr):
    print("Checking Materia v4 connection")

    resp = mr.request("{\"operation\":\"describe\", \"params\":{}}")
    if not resp.__contains__("strategy_node"):
        raise Exception("Materia v4 is not available")

    print("  - v4 connection OK")

def checkMateriaV5():
    print("Checking Materia v5 connection")
    
    try:
        # Try different possible v5 endpoints
        v5_urls = [
            "http://ramsnake.net:3000/health",
            "https://ramsnake.net/health", 
            "http://ramsnake.net/health"
        ]
        
        for url in v5_urls:
            try:
                response = requests.get(url, timeout=10)
                if response.status_code == 200:
                    print(f"  - v5 connection OK ({url})")
                    return url.replace('/health', '')
            except:
                continue
        
        print("  - v5 connection not available (this is OK if v5 is not deployed yet)")
        return None
        
    except Exception as e:
        print(f"  - v5 check failed: {e}")
        return None

def authenticateV5(base_url, v5_username, v5_password):
    """Authenticate with Materia v5 and return JWT token"""
    try:
        auth_url = f"{base_url}/api/auth/login"
        auth_data = {
            "username": v5_username,
            "password": v5_password
        }
        
        response = requests.post(auth_url, json=auth_data, timeout=10)
        if response.status_code == 200:
            result = response.json()
            if 'token' in result:
                print(f"  - v5 authentication successful")
                return result['token']
        
        print(f"  - v5 authentication failed: {response.status_code}")
        return None
        
    except Exception as e:
        print(f"  - v5 authentication error: {e}")
        return None

def doBackupV5(v5_base_url, v5_username, v5_password, ssh_username, ssh_password):
    """Backup v5 using the same pattern as v4: API call + SSH download"""
    print("  - Backing up Materia v5...")
    
    try:
        # Step 1: Authenticate with v5 API (like v4's encrypted API call)
        print("    - Authenticating with v5 API...")
        v5_token = authenticateV5(v5_base_url, v5_username, v5_password)
        
        if not v5_token:
            raise Exception("v5 authentication failed")
        
        # Step 2: Request backup creation via API (like v4's backup operation)
        print("    - Requesting backup creation...")
        backup_url = f"{v5_base_url}/api/backup/create"
        headers = {
            "Authorization": f"Bearer {v5_token}",
            "Content-Type": "application/json"
        }
        
        backup_response = requests.post(backup_url, headers=headers, timeout=30)
        if backup_response.status_code != 200:
            raise Exception(f"v5 backup API failed: {backup_response.status_code}")
        
        result = backup_response.json()
        if not result.get('success'):
            raise Exception(f"v5 backup failed: {result.get('message', 'Unknown error')}")
        
        backup_filename = result.get('backupFile')
        if not backup_filename:
            raise Exception("v5 backup API did not return backup filename")
        
        print(f"    - Backup created on server: {backup_filename}")
        
        # Step 3: Download backup file via SSH (like v4's SFTP download)
        print("    - Downloading backup file via SSH...")
        uri = "ramsnake.net"
        transport = paramiko.Transport((uri, 22))
        transport.connect(None, ssh_username, ssh_password)
        sftp = paramiko.SFTPClient.from_transport(transport)
        
        remote_backup_path = f"/materia/backups/{backup_filename}"
        local_backup_path = f"./materia_v5.bu{str(time.time())}.db"
        
        sftp.get(remote_backup_path, local_backup_path)
        sftp.close()
        transport.close()
        
        print(f"    - v5 backup saved to: {local_backup_path}")
        return True
        
    except Exception as e:
        print(f"    - v5 backup failed: {e}")
        return False

def doBackup(mr, uname, password, v5_base_url=None, v5_username="snake", v5_password=None):
    print("Doing backup")

    # Backup old Materia v4 system
    print("  - Backing up Materia v4...")
    resp = mr.request("{\"operation\":\"backup\", \"params\":{}}")
    if not resp.__contains__("success"):
        raise Exception("Materia v4 backup failed")

    uri = "ramsnake.net"
    transport = paramiko.Transport((uri,22))
    transport.connect(None,uname,password)
    sftp = paramiko.SFTPClient.from_transport(transport)

    # Download v4 backup
    filepath = "/materia/materia.back"
    localpath = "./materia_v4.bu" + str(time.time())
    sftp.get(filepath,localpath)
    print(f"  - v4 backup saved to: {localpath}")

    # Backup new Materia v5 system (using same pattern as v4)
    if v5_base_url and v5_password:
        doBackupV5(v5_base_url, v5_username, v5_password, uname, password)
    else:
        print("  - Skipping v5 backup (no v5 credentials provided)")

    sftp.close()
    transport.close()
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

    make_news.do(mr)

    print("Done")

def main():
    passwordFileName = sys.argv[1]
    file = open(passwordFileName, mode='r')
    lines = file.readlines()
    passwd = lines[0].replace("\n", "")      # v4 API password
    uname = lines[1].replace("\n", "")       # SSH username
    sshpass = lines[2].replace("\n", "")     # SSH password
    
    # Optional v5 credentials (if provided)
    v5_username = "snake"  # default
    v5_password = None
    if len(lines) >= 5:
        v5_username = lines[3].replace("\n", "")  # v5 username
        v5_password = lines[4].replace("\n", "")  # v5 password
    
    file.close()

    mr = MateriaConnection(passwd)

    # Check both systems
    checkMateria(mr)
    v5_base_url = checkMateriaV5()
    
    # Backup both systems
    doBackup(mr, uname, sshpass, v5_base_url, v5_username, v5_password)
    
    # Continue with other operations (using v4 for now)
    runInvestSurey(mr)
    runInvestAnalisys(mr)

    #collectNews(mr)

if __name__=="__main__":
    main()
