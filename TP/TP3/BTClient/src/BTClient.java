import javax.microedition.lcdui.*;
import javax.microedition.midlet.MIDlet;
import java.lang.Runnable;
import java.io.*;
import javax.bluetooth.*;
import javax.microedition.io.Connector;
import javax.microedition.io.StreamConnection;
import javax.microedition.io.StreamConnectionNotifier;

public class BTClient extends MIDlet implements CommandListener, Runnable {

  String name,
         address,
         connstr,
         message = "Test";

  public void run() {
    try {
      // Get a connection to the local device
      LocalDevice localdevice = LocalDevice.getLocalDevice();

      name = localdevice.getFriendlyName();
      address = localdevice.getBluetoothAddress();

      // Connect to the server using the same UID string
      DiscoveryAgent dagent = localdevice.getDiscoveryAgent();
      connstr = dagent.selectService(new UUID("86b4d", false),
          ServiceRecord.NOAUTHENTICATE_NOENCRYPT, false);
      StreamConnection stream = (StreamConnection)Connector.open(connstr);
      OutputStream out = stream.openOutputStream();

      // Send the message to the server
      out.write(message.getBytes());

      // Close the connections
      out.close();
      stream.close();
    }
    catch (IOException e) {}
  }

  public void startApp() {
    Display display = Display.getDisplay(this);

    Form mainForm = new Form("BTServer");

    display.setCurrent(mainForm);

    new Thread(this).start();

  }

  public void pauseApp() {}

  public void destroyApp(boolean unconditionnal) {}

  public void commandAction(Command c, Displayable s) {}
}
