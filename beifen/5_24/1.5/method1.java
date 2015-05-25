package algorithm;
import java.util.Arrays;
import java.util.Scanner;

public class myFirstApp {
	public static void main(String[] args){
		Scanner user_input = new Scanner(System.in);
		String str = user_input.nextLine();
		String print_str= compress(str);
		System.out.println(print_str);
	}
	public static String compress(String str){
		StringBuffer buffer = new StringBuffer();
		char first;
		int count =1;
		first = str.charAt(0);
		for(int i=1; i<str.length(); i++ ){
			if(first == str.charAt(i)){
				count ++;
			}
			else{
				buffer.append(first);
				buffer.append(Integer.toString(count));
				count = 1;
			}
			first = str.charAt(i);
		}
		buffer.append(first);
		buffer.append(count);
		if(buffer.toString().length()>str.length() )
			return str;
		else
			return buffer.toString();
	}
}
