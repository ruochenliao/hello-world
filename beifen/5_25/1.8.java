package algorithm;
import java.util.Arrays;
import java.util.Scanner;

public class myFirstApp {
	public static void main(String[] args){
		Scanner user_input = new Scanner(System.in);
		String str1 = user_input.nextLine();
		String str2 = user_input.nextLine();
		boolean isRotate = ifRotate(str1,str2);
		System.out.println(isRotate);
	}
	//str1 is the original string, str2 is the rotated string
	public static boolean ifRotate(String str1, String str2){
		if(str1.length() == str2.length() && str1.length()>0){
			String str = str1+str1;
			return ifSubString(str,str2);
		}
		else 
			return false;
	}
	//check str2 is the subString of str1
	public static boolean ifSubString(String str1, String str2){
		return str1.contains(str2);
	}
}
