package algorithm;
import java.util.Arrays;
import java.util.Scanner;

public class myFirstApp {
	public static void main(String[] args){
		//int[] a = {1,2,3};
		//System.out.println(a.length);
		Scanner user_input = new Scanner(System.in);
		String str = user_input.nextLine();
		String N_str = user_input.nextLine();
		int N = Integer.parseInt(N_str);
		char[] one_demention = str.toCharArray();
		int length = one_demention.length;
		char[][] two_demention = new char[N][N];
		int k = 0;
		outerloop:
		for(int i = 0; i<N;i++){
			for(int j = 0; j< N; j++){
				two_demention[i][j] = one_demention[k];
				
				if( k < length-1 )
					k++;
				else
					break outerloop;
			}
		}
		System.out.println(Arrays.deepToString(two_demention));
		myFirstApp.rotate(two_demention,N);
		System.out.println(Arrays.deepToString(two_demention));
	}
	public static void rotate( char array[][], int N){
		char temp;
		//temp = array[0][0];
		int first, last;
		for(int layer = 0; layer< N/2; layer++){
			first = layer;
			last = N -layer-1;
			for(int i = first; i < last; i++){
				int offset = i - first;
				temp = array[first][i];
				array[first][i] = array[last - offset][first];
				array[last - offset][first] = array[last][last-offset];
				array[last][last -offset] = array[i][last];
				array[i][last] = temp;
			}
		}
	}
}
