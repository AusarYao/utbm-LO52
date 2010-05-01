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
    // We connect to our local device
    LocalDevice localdevice = LocalDevice.getLocalDevice();
    name = localdevice.getFriendlyName();
    address = localdevice.getBluetoothAddress();

    // Try to run as client, connecting to a server
    try
      run_client();
    // If an exception is raised, we run as server
    catch (IOException e)
      run_server();
  }

  public void run_client() {
    // Connect to a server using UID.
    DiscoveryAgent dagent = localdevice.getDiscoveryAgent();
    connstr = dagent.selectService(new UUID("86b4d", false),
        ServiceRecord.NOAUTHENTICATE_NOENCRYPT,
        false);

    // Open connection, and send a message to the server
    StreamConnection stream = (StreamConnection)Connector.open(connstr);
    OutputStream out = stream.openOutputStream();
    out.write(message.getBytes());

    // Close the connections
    out.close();
    stream.close();
  }



  public void run_server() {

    // Set ourselves discoverable
    localdevice.setDiscoverable(DiscoveryAgent.GIAC);

    // Listen to connections on our UID
    StreamConnectionNotifier scn =
      (StreamConnectionNotifier)Connector.open("btspp://localhost:86b4d");
    StreamConnection client = scn.acceptAndOpen();
    InputStream input = client.openInputStream();

    // Print data received
    int data;
    ByteArrayOutputStream array = new ByteArrayOutputStream();
    while((data = input.read()) > -1)
      array.write(data);
    System.out.println(array.toString());

    // Close connections
    input.close();
    client.close();
    scn.close();
  }

  public void startApp() {

    Display display = Display.getDisplay(this);
    Form mainForm = new Form("BTCliServ");
    display.setCurrent(mainForm);
    new Thread(this).start();

  }

  public void pauseApp() {}

  public void destroyApp(boolean unconditionnal) {}

  public void commandAction(Command c, Displayable s) {}
}
