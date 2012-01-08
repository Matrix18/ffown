# Experiment with wxPython's HtmlWindow
# tested with Python24 and wxPython26    vegaseat   17may2006
 #!/usr/bin/python
import wx
import  wx.html
import os 

class ff_app(wx.PySimpleApp):
    def __init__(self):
        wx.PySimpleApp.__init__(self)
        os.chdir("./html")

class ff_html_win(wx.html.HtmlWindow):
    def __init__(self, parent, pos = (0, 1), size = (600,400)):
        wx.html.HtmlWindow.__init__(self, parent, -1, pos, size)
        if "gtk2" in wx.PlatformInfo: 
            self.SetStandardFonts() 


class ff_html_panel(wx.Panel):
    """
    class ff_html_panel inherits wx.Panel and auto load files from html/index.html
    """
    def __init__(self, parent, id):
        # default pos is (0, 0) and size is (-1, -1) which fills the frame
        wx.Panel.__init__(self, parent, id)
        self.SetBackgroundColour("write")
        html_content = "<h2>404 Page Not found ./index.htm</h2>"
        
        try:
            a = file("./index.html")
            html_content = a.read()
        except:
            pass

        self.html_win = ff_html_win(self, pos=(0,1), size=(800,600))
        self.html_win.SetRelatedFrame(parent, parent.GetTitle() + " -- %s")
        self.html_win.SetRelatedStatusBar(0)
        self.html_win.SetPage(str(html_content))


class ff_html_frame(wx.Frame): 
    def __init__(self, parent, title): 
        wx.Frame.__init__(self, parent, -1, title, size=(800,600)) 
        self.CreateStatusBar()
        self.html_apnel = ff_html_panel(self,-1)

app = ff_app()
# create a window/frame, no parent, -1 is default ID, title, size
frame = ff_html_frame(None, "FFOWN HTML Browser") 
# call the derived class, -1 is default ID
#html_apnel = ff_html_panel(frame,-1)
# show the frame
frame.Show(True)
# start the event loop
app.MainLoop()
