package algorithm;
import java.util.Arrays;
import java.util.Scanner;

public class myFirstApp {
	public static void main(String[] args){

		Scanner user_input = new Scanner(System.in);
		String str = user_input.nextLine();
		int M = Integer.parseInt(user_input.nextLine());
		int N = Integer.parseInt(user_input.nextLine());
		char[] one_demention = str.toCharArray();
		int length = one_demention.length;
		int[][] two_demention = new int[M][N];
		int k = 0;
		outerloop:
		for(int i = 0; i<M;i++){
			for(int j = 0; j< N; j++){
				two_demention[i][j] = (int)(one_demention[k] - '0');
				if( k < length-1 )
					k++;
				else
					break outerloop;
			}
		}
		System.out.println(Arrays.deepToString(two_demention));
		myFirstApp.set_0(two_demention);
		System.out.println(Arrays.deepToString(two_demention));
	}
	public static void set_0(int a[][]){
		int M = a.length;
		int N = a[0].length;
		boolean[] row = new boolean[M];
		boolean[] column = new boolean[N];
		for(int i=0; i<M; i++ ){
			for(int j =0; j<N; j++){
				if(a[i][j] == 0){
					row[i] = true;
					column[j] =true;
				}
			}
		}
		for(int i=0; i<M; i++){
			if( row[i] ){
				for(int j =0; j<N;j++ ){
					a[i][j] =0;
				}
			}
		}
		for(int j=0;j<N; j++){
			if( column[j] ){
				for(int i=0;i<M; i++){
					a[i][j] = 0;
				}
			}
		}
	}
}
