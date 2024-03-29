#
# MIT License
#
# Copyright (c) 2020 Denis Kovalchuk
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

import os
import logging
import sys
from pyftpdlib.authorizers import DummyAuthorizer
from pyftpdlib.handlers import FTPHandler
from pyftpdlib.servers import FTPServer

def main():
    if len(sys.argv) != 3:
        print("Usage: server.py port root_directory")
        return

    port = sys.argv[1]
    root_directory = sys.argv[2]

    # Add user with the following permissions:
    #   e - change directory (CWD, CDUP commands)
    #   l - list files (LIST, NLST, STAT, MLSD, MLST, SIZE commands)
    #   r - retrieve file from the server (RETR command)
    #   a - append data to an existing file (APPE command)
    #   d - delete file or directory (DELE, RMD commands)
    #   f - rename file or directory (RNFR, RNTO commands)
    #   m - create directory (MKD command)
    #   w - store a file to the server (STOR, STOU commands)
    #   M - change file mode / permission (SITE CHMOD command)
    authorizer = DummyAuthorizer()
    authorizer.add_user("user", "password", root_directory, perm = "elradfmwM")
    authorizer.add_user("alice", "password", root_directory, perm = "elradfmwM")

    handler = FTPHandler
    handler.authorizer = authorizer
    handler.banner = "FTP server is ready."

    if os.path.exists("pyftpd.log"):
        os.remove("pyftpd.log")

    logging.basicConfig(handlers = [
                            logging.FileHandler("pyftpd.log"),
                            logging.StreamHandler()],
                        level = logging.DEBUG)

    server = FTPServer(("127.0.0.1", port), handler)
    server = FTPServer(("::1", port), handler)
    server.serve_forever()

if __name__ == "__main__":
    main()