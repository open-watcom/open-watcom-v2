package sun.tools.debug;

import java.util.*;
import java.io.*;
import java.awt.*;
import java.applet.*;
import java.net.URL;
import java.net.MalformedURLException;
import sun.awt.image.URLImageSource;

class jvmhandler extends Thread {
    static BreakpointQueue	bkptQ;
    static native void BreakpointLoop( ThreadGroup t );

    jvmhandler() {
	super("jvmhelp breakpoint handler");
	bkptQ = new BreakpointQueue();
	bkptQ.reset();
//	bkptQ.the_bkptQ = bkptQ;
    }

    private boolean systemThread(Thread t) {
	String	tname = String.valueOf(t.getName());

	if( tname.equals("jvmhelp breakpoint handler") ||
	    tname.equals("jvmhelp communication thread") ||
	    tname.equals("jvmhelp loader thread") ||
	    tname.equals("clock handler") ||
	    tname.equals("Idle thrad") ) {
	    return true;
	}
	return false;
    }

    private void suspendThread(Thread t) {
	if (!systemThread(t)) {
	    try {
		t.suspend();
	    } catch (IllegalThreadStateException e) {
		;
	    }
	}
    }

    private void resumeThread(Thread t) {
        if (!systemThread(t)) {
	    try {
	        t.resume();
	    } catch (IllegalThreadStateException e) {
	        ;
	    }
	}
    }

    public void run() {
	BreakpointLoop( Thread.currentThread().getThreadGroup() );
    }
}

class jvmhelp extends Thread
{
    static native int EntryPoint();
    static native void LoadCallBack();
    static native void PlantAppletBreak( String s );
    static public Thread LoaderThread;
    static public Thread CommThread;

    static public void main( String [] parms )
    {
	jvmhandler b = new jvmhandler();
	b.setPriority( Thread.MAX_PRIORITY );
	System.loadLibrary("jvmhlp");
	b.start();
	CommThread = Thread.currentThread();
	CommThread.setName( "jvmhelp communication thread" );
	EntryPoint();
    }
    static public void Load()
    {
	LoaderThread = new jvmhelp();
	LoaderThread.start(); // invokes "run" method below
    }
    public void run()
    {
	setName("main");
	LoadCallBack();
    }

    static Hashtable systemParam = new Hashtable();

    static {
	systemParam.put("codebase", "codebase");
	systemParam.put("code", "code");
	systemParam.put("alt", "alt");
	systemParam.put("width", "width");
	systemParam.put("height", "height");
	systemParam.put("align", "align");
	systemParam.put("vspace", "vspace");
	systemParam.put("hspace", "hspace");
    }

    static public void ParseHTML( String [] urlName )
    {
	LoaderThread.setName("jvmhelp loader thread");
	System.out.println(urlName[0]);
	try {
	    URL url;

	    if (urlName[0].indexOf(':') <= 1) {
		url = new URL("file:" + System.getProperty("user.dir").replace(File.separatorChar, '/') + "/");
		url = new URL(url, urlName[0]);
	    } else {
		url = new URL(urlName[0]);
	    }
	    parse(url);
	} catch (MalformedURLException e) {
	    System.out.println("Bad URL: " + urlName[0]
			       + " (" + e.getMessage() + ")");
	} catch (IOException e) {
	    System.out.println("I/O exception while reading: " + e.getMessage());
	    if (urlName[0].indexOf(':') < 0) {
		System.out.println("Make sure that " + urlName[0] + " is a file and is readable.");
	    } else {
		System.out.println("Is " + urlName[0] + " the correct URL?");
	    }
	}
    }
    static int c;

    static void skipSpace(InputStream in) throws IOException {
	while((c >= 0) &&
	      ((c == ' ') || (c == '\t') || (c == '\n') || (c == '\r'))) {
	    c = in.read();
	}
    }

    static void printTag(PrintStream out, Hashtable atts) {
	out.print("<applet");

	String v = (String)atts.get("codebase");
	if (v != null) {
	    out.print(" codebase=\"" + v + "\"");
	}

	v = (String)atts.get("code");
	if (v == null) {
	    v = "applet.class";
	}
	out.print(" code=\"" + v + "\"");

	v = (String)atts.get("width");
	if (v == null) {
	    v = "150";
	}
	out.print(" width=" + v);

	v = (String)atts.get("height");
	if (v == null) {
	    v = "100";
	}
	out.print(" height=" + v);

	v = (String)atts.get("name");
	if (v != null) {
	    out.print(" name=\"" + v + "\"");
	}
	out.println(">");

	// A very slow sorting algorithm
	int len = atts.size();
	String params[] = new String[len];
	len = 0;
	for (Enumeration e = atts.keys() ; e.hasMoreElements() ;) {
	    String param = (String)e.nextElement();
	    int i = 0;
	    for (; i < len ; i++) {
		if (params[i].compareTo(param) >= 0) {
		    break;
		}
	    }
	    System.arraycopy(params, i, params, i + 1, len - i);
	    params[i] = param;
	    len++;
	}

	for (int i = 0 ; i < len ; i++) {
	    String param = params[i];
	    if (systemParam.get(param) == null) {
		out.println("<param name=" + param + " value=\"" + atts.get(param) + "\">");
	    }
	}
	out.println("</applet>");
    }

    static Hashtable scanTag(InputStream in) throws IOException {
	Hashtable atts = new Hashtable();
	skipSpace(in);
	while (c >= 0 && c != '>') {
	    String att = scanIdentifier(in);
	    String val = "";
	    skipSpace(in);
	    if (c == '=') {
		int quote = -1;
		c = in.read();
		skipSpace(in);
		if ((c == '\'') || (c == '\"')) {
		    quote = c;
		    c = in.read();
		}
		StringBuffer buf = new StringBuffer();
		while ((c > 0) &&
		       (((quote < 0) && (c != ' ') && (c != '\t') && 
			 (c != '\n') && (c != '\r') && (c != '>'))
			|| ((quote >= 0) && (c != quote)))) {
		    buf.append((char)c);
		    c = in.read();
		}
		if (c == quote) {
		    c = in.read();
		}
		skipSpace(in);
		val = buf.toString();
	    }
	    //System.out.println("PUT " + att + " = '" + val + "'");
	    atts.put(att, val);
	    skipSpace(in);
	}
	return atts;
    }
    static String scanIdentifier(InputStream in) throws IOException {
	StringBuffer buf = new StringBuffer();
	while (true) {
	    if ((c >= 'a') && (c <= 'z')) {
		buf.append((char)c);
		c = in.read();
	    } else if ((c >= 'A') && (c <= 'Z')) {
		buf.append((char)('a' + (c - 'A')));
		c = in.read();
	    } else if ((c >= '0') && (c <= '9')) {
		buf.append((char)c);
		c = in.read();
	    } else {
		return buf.toString();
	    }
	}
    }

    static void parse(URL url) throws IOException
    {
	InputStream in = url.openStream();
	Hashtable atts = null;
	c = in.read();
	while (c >= 0) {
	    if (c == '<') {
		c = in.read();
		if (c == '/') {
		    c = in.read();
		    String nm = scanIdentifier(in);
		    if (nm.equals("applet")) {
			if (atts != null) {
			    PlantAppletBreak( (String)atts.get("code") );
			}
			atts = null;
		    }
		} else {
		    String nm = scanIdentifier(in);
		    if (nm.equals("param")) {
			Hashtable t = scanTag(in);
			String att = (String)t.get("name");
			if (att == null) {
			    System.out.println("Warning: <param name=... value=...> tag requires name attribute.");
			} else {
			    String val = (String)t.get("value");
			    if (val == null) {
				System.out.println("Warning: <param name=... value=...> tag requires value attribute.");
			    } else if (atts != null) {
				atts.put(att.toLowerCase(), val);
			    } else {
				System.out.println("Warning: <param> tag outside <applet> ... </applet>.");
			    }
			}
		    } else if (nm.equals("applet")) {
			atts = scanTag(in);
			if (atts.get("code") == null) {
			    System.out.println("Warning: <applet> tag requires code attribute.");
			    atts = null;
			} else if (atts.get("width") == null) {
			    System.out.println("Warning: <applet> tag requires width attribute.");
			    atts = null;
			} else if (atts.get("height") == null) {
			    System.out.println("Warning: <applet> tag requires height attribute.");
			    atts = null;
			}
		    } else if (nm.equals("app")) {
			System.out.println("Warning: <app> tag no longer supported, use <applet> instead:");
			Hashtable atts2 = scanTag(in);
			nm = (String)atts2.get("class");
			if (nm != null) {
			    atts2.remove("class");
			    atts2.put("code", nm + ".class");
			}
			nm = (String)atts2.get("src");
			if (nm != null) {
			    atts2.remove("src");
			    atts2.put("codebase", nm);
			}
			if (atts2.get("width") == null) {
			    atts2.put("width", "100");
			}
			if (atts2.get("height") == null) {
			    atts2.put("height", "100");
			}
			printTag(System.out, atts2);
			System.out.println();
		    }
		} 
	    } else {
		c = in.read();
	    }
	}
	in.close();
    }
}
