import javax.microedition.lcdui.*;
import javax.microedition.midlet.MIDlet;

public class TinyMIDlet extends MIDlet implements CommandListener {
	Command exitCommand,
		backCommand,
		hwCommand;

	public void startApp() {
		Display display = Display.getDisplay(this);
		Form mainForm = new Form("TinyMIDlet");

		mainForm.append("Welcome to the world of MIDlets!");

		exitCommand = new Command("Exit", Command.EXIT, 0);
		hwCommand = new Command("Hello", Command.ITEM, 0);
		backCommand = new Command("Back", Command.ITEM, 0);

		mainForm.addCommand(exitCommand);
		mainForm.addCommand(hwCommand);

		mainForm.setCommandListener(this);

		display.setCurrent(mainForm);
	}

	public void pauseApp () {}

	public void destroyApp(boolean unconditional) {}

	public void commandAction(Command c, Displayable s) {
		if (c.getCommandType() == Command.EXIT)
			notifyDestroyed();
		else if (c == hwCommand) {
			Display display = Display.getDisplay(this);
			Form mainForm = new Form("TinyMIDlet");
			mainForm.append("Hello World!");

			mainForm.addCommand(backCommand);
			mainForm.setCommandListener(this);

			display.setCurrent(mainForm);
		}
		else if (c == backCommand) {
			Display display = Display.getDisplay(this);
			Form mainForm = new Form("TinyMIDlet");

			mainForm.append("Welcome to the world of MIDlets!");

			mainForm.addCommand(exitCommand);
			mainForm.addCommand(hwCommand);

			mainForm.setCommandListener(this);

			display.setCurrent(mainForm);
		}
	}
}
