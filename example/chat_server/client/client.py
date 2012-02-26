import socket
import struct
import time
import thread 
import threading

class client_t:
    uid = 0
    def __init__(self, host_, port_, handler_):
        self.host  = host_
        self.port  = port_
        self.handler = handler_
        self.mutex = threading.Lock()
        self.status = "broken"

    def handle_broken(self):
        self.close()
        self.status = "broken"
        self.handler.handle_broken(self)
        #print "handle_broken::handle_broken"

    def connect(self):
        try:
            self.socket = socket.socket()
            self.socket.connect((self.host, self.port))
            self.status = "ok"
        except:
            self.handle_broken()

    def dump(self):
        print "host:port", self.host, self.port

    def send_msg(self, cmd_, content_):
        self.mutex.acquire()
        try:
            #head = struct.pack('IHH', len(content_), cmd_, 0)
            dest = str(len(content_)) + '\r\n' + content_
            self.socket.sendall(dest)
            #self.socket.sendall((head.decode() + content_).encode())
        except:
            self.handle_broken()
        self.mutex.release()

    def read_some(self):
        try:
            buff = self.socket.recv(1024*8)
            dest = buff.split('\r\n')
            self.handler.handle_msg(self, dest[0], dest[1])
        except:
            self.handle_broken()


    def close(self):
        self.socket.close()

    def loop(self, xx):
        loop = 10
        while loop > 0  and self.status != "broken":
            loop = loop - 1
            self.read_some()
    def run(self):
        thread.start_new_thread(self.loop, (self,))

class msg_handler_t:
    def handle_broken(self, client_):
        print "msg_handler_t:handle_broken"

    def handle_msg(self, client_, cmd_, content):
        try:
            result = eval(content_)

            register_dict =  {
                "user_login_ret_t" : self.handle_login,
                "echo_result"  : self.handle_echo
            }
            for key in result:
                if register_dict.get(key) != None:
                    register_dict[key](client_, result)

        except:
            print "handle_msgXXX....cmd:", cmd_, "not supported!", "body:", content_
            return
        
    def handle_login(self, client_, result_):
        client_.uid = result_["uid"]
        print "handle_login...uid:", client_.uid
        
        pass
    def handle_echo(self, client_, result_):
        print "handle_echo...content:<", result_['content'], ">"
        
        pass
    def handle_broadcast():
        pass

def test():
    host = "127.0.0.1"
    port = 10241
    
    h = msg_handler_t()
    c = client_t(host, port, h)
    cmd = 0
    c.connect()
    c.send_msg(cmd, '{"login_req_t":{"uid":123}}')
    c.dump()
    c.read_some()
    c.run()
    get_cmd(c)

def get_cmd(c):
    cmd = 1
    while cmd:
        cmd = input("input cmd:")
        c.send_msg(cmd, '{"login_req_t":{"uid":123}}')

def exe(cmd):
    shell_cmd = cmd + ' > /tmp/tmp_result'
    os.system(shell_cmd)
    f = open('/tmp/tmp_result')
    result = f.read()
    return result
    
if __name__ == "__main__":
    test()
    

