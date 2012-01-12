from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4.QtWebKit import *

example = """
           <script>function message() { return "Clicked!"; }</script>
           <h1>QtWebKit + Python sample program</h1>
           <input type="button" value="Click JavaScript!" 
                  onClick="alert('[javascript] ' + message())"/>
           <input type="button" value="Click Python!" 
                  onClick="python.alert('[python] ' +
                                        python.message())"/>
           <br />
           <iframe src="http://www.baidu.com/"
                   width="100%" height="100%"
                   scrolling="true"
                   frameborder="0"
                   align="center"></iframe>
"""
class BrowserScreen(QWebView):
    def __init__(self):
        QWebView.__init__(self)

        self.resize(800, 600)
        self.show()

        html_content = "<h2>404 Page Not found ./index.htm</h2>"
        
        try:
            a = file("./index.html")
            html_content = a.read()
        except:
            pass
        
        self.setHtml(html_content)
        self.createTrayIcon()
        self.trayIcon.show()
    def createTrayIcon(self):
        self.trayIcon = QSystemTrayIcon(self)
        self.trayIcon.setIcon(QIcon("images/xm.ico"))
    def showMessage(self, msg):
        self.trayIcon.showMessage("This is Python", msg,
            QSystemTrayIcon.MessageIcon(0), 15 * 1000)

class PythonJS(QObject):
    __pyqtSignals__ = ("contentChanged(const QString &)")
    @pyqtSignature("QString")
    def alert(self, msg):
        self.emit(SIGNAL('contentChanged(const QString &)'), msg)

    @pyqtSignature("", result="QString")
    def message(self):
        return "Click!"

def run_loop():
    import sys
    import os
    os.chdir("./html")
    app = QApplication(sys.argv)

    browser = BrowserScreen()
    pjs = PythonJS()
    browser.page().mainFrame().addToJavaScriptWindowObject("python", pjs)

    QObject.connect(pjs, SIGNAL("contentChanged(const QString &)"),
                    browser.showMessage)

    sys.exit(app.exec_())

    
