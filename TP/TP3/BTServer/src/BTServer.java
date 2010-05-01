import javax.microedition.lcdui.*;
import javax.microedition.midlet.MIDlet;
import java.lang.Runnable;
import java.io.*;
import javax.bluetooth.*;
import javax.microedition.io.Connector;
import javax.microedition.io.StreamConnection;
import javax.microedition.io.StreamConnectionNotifier;

public class BTServer extends MIDlet implements CommandListener, Runnable {

  String name,
         address;

  public void run() {
    try {
      // Make the device discoverable to the client
      LocalDevice localdevice = LocalDevice.getLocalDevice();
      localdevice.setDiscoverable(DiscoveryAgent.GIAC);

      name = localdevice.getFriendlyName();
      address = localdevice.getBluetoothAddress();

      // Create a server object
      StreamConnectionNotifier scn =
        (StreamConnectionNotifier)Connector.open("btspp://localhost:86b4d");

      // Accept connections from clients
      StreamConnection client = scn.acceptAndOpen();
      InputStream input = client.openInputStream();
      int data;
      ByteArrayOutputStream array = new ByteArrayOutputStream();

      // We read data until EOF
      while((data = input.read()) > -1)
        array.write(data);

      // And now we display it
      System.out.println(array.toString());

      // Finally, we close connections
      input.close();
      client.close();
      scn.close();
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
