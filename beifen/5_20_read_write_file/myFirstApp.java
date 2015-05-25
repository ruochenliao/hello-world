package algorithm;
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.ObjectOutputStream;
import java.util.ArrayList;
import java.util.Calendar;

import javax.sound.midi.MidiEvent;
import javax.sound.midi.MidiSystem;
import javax.sound.midi.Sequence;
import javax.sound.midi.Sequencer;
import javax.sound.midi.ShortMessage;
import javax.sound.midi.Track;

public class myFirstApp {
	public static void main(String[] args){
		//myFirstApp.readFileByLines("file.txt");
		try{
			File myFile = new File("file.txt");
			FileWriter fileWriter = new FileWriter(myFile);
			BufferedWriter writer = new BufferedWriter(fileWriter);
			writer.write("aaaaaaaaaaaaaa");
			writer.close();
			
			File myFile_r = new File("file.txt");
			FileReader fileReader = new FileReader(myFile_r);
			BufferedReader reader = new BufferedReader(fileReader);
			String line = null;
			while( (line = reader.readLine())!= null ){
				System.out.println(line);
			}
			reader.close();			
		}catch(Exception ex){
			ex.printStackTrace();
		}

	}
}

