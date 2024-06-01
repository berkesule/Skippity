#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include<math.h>
#define max(a,b) ((a) > (b) ? a : b)
#define min(a,b) ((a) < (b) ? a : b)
#define MAX_DEPTH 5

/*
Puan hesabý:
yenen her tas icin 10 puan
eger bir set tamamlarnirsa yani a b c d e nin her biri en az 1 ise 100 puan eklenir ve abcde 1 er azaltilir
*/
typedef struct {
    int x1, y1, x2, y2;
} Move;
typedef struct {
    int a;
    int b;
    int c;
    int d;
    int e;
    int sum;
    int score;
} Oyuncu;

void printBoard(char** board, int size);
int isValidMove(char** board, int size, int x1, int y1, int x2, int y2, int* captureX, int* captureY);
int checkForValidMoves(char** board, int size);
void captureSkipper(char** board, Oyuncu* oyuncu, char skipper);
void checkAndResetSkippers(Oyuncu* oyuncu);
void freeBoard(char** board, int size);
void copyBoard(char*** newBoard, char** board, int size);
int evaluateBoard(char** board, int size, Oyuncu oyuncu1, Oyuncu oyuncu2);
int minimax(char** board, int size, int depth, int isMaximizing, Oyuncu oyuncu1, Oyuncu oyuncu2, int turn,int prevX, int prevY,int continueMove);
Move findBestMove(char** board, int size, Oyuncu oyuncu1, Oyuncu oyuncu2, int turn, int prevX, int prevY);
int evaluateBoard(char** board, int size, Oyuncu oyuncu1, Oyuncu oyuncu2) {
    return oyuncu2.score - oyuncu1.score;
}
/*
minmax ecaluateBoard ve findBestMove bilgisayara karsi mod icin gereken fonksiyonlar. Algoritma su sekilde calisiyor:
oyuncu1(insan)  ve oyuncu 2(bilgisayar) arasýndaki puan farkýnýn bilgisayar lehine fazla olmasý icin hesaplamalar yapiyor ve bilgisayar icin en iyi hamleyi buluyor.
maxdepth'i arttirdikca daha iyi oynuyor fakat yavasliyor.
*/
int minimax(char** board, int size, int depth, int isMaximizing, Oyuncu oyuncu1, Oyuncu oyuncu2, int turn, int prevX, int prevY, int continueMove) {
    int x1, x2, y1, y2, oldscore, newscore,eval,minEval,maxEval;
    int prevxsize, prevysize;
    char skipper;
    char** newBoard;
    if (depth == 0 || !checkForValidMoves(board, size)) {
        return evaluateBoard(board, size, oyuncu1, oyuncu2);
    }
    ///asagidaki if elsin amaci art arda hamlede 1 onceki hamledeki tasla oynanmasini saglamak
    if (prevX != -1) { 
        prevxsize = prevX + 1;
        prevysize = prevY + 1;
    } else {
    	prevX = 0;
    	prevY = 0;
        prevxsize = size;
        prevysize = size;
    }

    if (isMaximizing) {///bilgisayarin max puan almasi icin
        maxEval = INT_MIN;///baslangicta sonsuz kucuge esitledim
        for (x1 = prevX; x1 < prevxsize; x1++) {
            for (y1 = prevY; y1 < prevysize; y1++) {
                if (board[x1][y1] != ' ') {
                    for (x2 = 0; x2 < size; x2++) {
                        for (y2 = 0; y2 < size; y2++) {
                            int captureX, captureY;
                            if (isValidMove(board, size, x1, y1, x2, y2, &captureX, &captureY)) {
                                
                                copyBoard(&newBoard, board, size);
                                skipper = board[captureX][captureY];
                                newBoard[x2][y2] = newBoard[x1][y1];
                                newBoard[x1][y1] = ' ';
                                newBoard[captureX][captureY] = ' ';

                                Oyuncu newOyuncu1 = oyuncu1;
                                captureSkipper(newBoard, &newOyuncu1, skipper);
                                checkAndResetSkippers(&newOyuncu1);

                                eval = evaluateBoard(board,size,oyuncu1,oyuncu2);
                                if (checkAdditionalMove(newBoard, size, x2, y2)) {
                                    eval = minimax(newBoard, size, depth - 1, 1, newOyuncu1, oyuncu2, turn, x2, y2, 1);
                                } else {
                                    eval = minimax(newBoard, size, depth - 1, 0, newOyuncu1, oyuncu2, 1 - turn, -1, -1, 0);
                                }

                                freeBoard(newBoard, size);
                                maxEval = max(maxEval, eval);
                            }
                        }
                    }
                }
            }
        }
        return maxEval;
    } else {///oyuncunun min puan almasi icin
        minEval = INT_MAX;///baslangicta sonsuza esitledim
        for (x1 = prevX; x1 < prevxsize; x1++) {
            for (y1 = prevY; y1 < prevysize; y1++) {
                if (board[x1][y1] != ' ') {
                    for (x2 = 0; x2 < size; x2++) {
                        for (y2 = 0; y2 < size; y2++) {
                            int captureX, captureY;
                            if (isValidMove(board, size, x1, y1, x2, y2, &captureX, &captureY)) {
                                char** newBoard;
                                copyBoard(&newBoard, board, size);
                                char skipper = board[captureX][captureY];
                                newBoard[x2][y2] = newBoard[x1][y1];
                                newBoard[x1][y1] = ' ';
                                newBoard[captureX][captureY] = ' ';

                                Oyuncu newOyuncu2 = oyuncu2;
                                captureSkipper(newBoard, &newOyuncu2, skipper);
                                checkAndResetSkippers(&newOyuncu2);

                                eval = evaluateBoard(board,size,oyuncu1,oyuncu2);
                                if (checkAdditionalMove(newBoard, size, x2, y2)) {
                                    eval = minimax(newBoard, size, depth - 1, 0, oyuncu1, newOyuncu2, turn, x2, y2, 1);
                                } else {
                                    eval = minimax(newBoard, size, depth - 1, 1, oyuncu1, newOyuncu2, 1 - turn, -1, -1, 0);
                                }

                                freeBoard(newBoard, size);
                                minEval = min(minEval, eval);
                            }
                        }
                    }
                }
            }
        }
        return minEval;
    }
}
Move findBestMove(char** board, int size, Oyuncu oyuncu1, Oyuncu oyuncu2, int turn, int prevX, int prevY) {
    int x1, x2, y1, y2,moveValue;;
    int bestValue = INT_MIN;
    Move bestMove;
    int prevxsize, prevysize;
    char** newBoard;
    char skipper;
    ///asagidaki if elsin amaci art arda hamlede 1 onceki hamledeki tasla oynanmasini saglamak
    if (prevX != -1) {
    
        prevxsize = prevX + 1;
        prevysize = prevY + 1;
    } else {
    	prevX = 0;
    	prevY = 0;
        prevxsize = size;
        prevysize = size;
    }

    for (x1 = prevX; x1 < prevxsize; x1++) {
        for (y1 = prevY; y1 < prevysize; y1++) {
            if (board[x1][y1] != ' ') {
                for (x2 = 0; x2 < size; x2++) {
                    for (y2 = 0; y2 < size; y2++) {
                        int captureX, captureY;
                        if (isValidMove(board, size, x1, y1, x2, y2, &captureX, &captureY)) {
                            copyBoard(&newBoard, board, size);
                            skipper = board[captureX][captureY];
                            newBoard[x2][y2] = newBoard[x1][y1];
                            newBoard[x1][y1] = ' ';
                            newBoard[captureX][captureY] = ' ';

                            Oyuncu newOyuncu1 = oyuncu1;
                            captureSkipper(newBoard, &newOyuncu1, skipper);
                            checkAndResetSkippers(&newOyuncu1);

                            
                            if (checkAdditionalMove(newBoard, size, x2, y2)) {
                                moveValue = minimax(newBoard, size, MAX_DEPTH - 1, 0, newOyuncu1, oyuncu2, turn, x2, y2, 1);
                            } else {
                                moveValue = minimax(newBoard, size, MAX_DEPTH - 1, 0, newOyuncu1, oyuncu2, 1 - turn, -1, -1, 0);
                            }

                            freeBoard(newBoard, size);

                            if (moveValue > bestValue) {
                                bestValue = moveValue;
                                bestMove.x1 = x1;
                                bestMove.y1 = y1;
                                bestMove.x2 = x2;
                                bestMove.y2 = y2;
                            }
                        }
                    }
                }
            }
        }
    }

    return bestMove;
}


void freeBoard(char** board, int size) {
    int i;
    for (i = 0; i < size; i++) {
        free(board[i]);
    }
    free(board);
}
void printBoard(char** board, int size) {
    int i, j;
    printf("  ");
    for (i = 0; i < size; i++) {
        printf(" %2d", i);
    }
    printf("\n");
    for (i = 0; i < size; i++) {
        printf("%2d ", i);
        for (j = 0; j < size; j++) {
            printf(" %c ", board[i][j]);
        }
        printf("\n");
    }
}

char** createBoard(int size) {
    int i;
    char** board = (char**)calloc(size, sizeof(char*));
    for (i = 0; i < size; i++) {
        board[i] = (char*)calloc(size, sizeof(char));
    }
    return board;
}

void fillBoard(char** board, int size) { ///tahtayi rastgele doldurmak icin
    char letters[] = { 'A', 'B', 'C', 'D', 'E' };
    int numLetters = sizeof(letters) / sizeof(letters[0]);
    int totalCells = size * size - 4; // 4 bos hucre
    int numEach = totalCells / numLetters;
    int extra = totalCells % numLetters;///5 tane tas var fakat mesela tahta boyutu 10 olursa 100-4 ten 96 tas gerek 96 5 e bolunurse 1 kalýr o kalan extra icin
    int i, j, letterIndex;

    int* count = (int*)calloc(numLetters, sizeof(int));
    srand(time(NULL));

    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            if ((i == size / 2 || i == size / 2 - 1) && (j == size / 2 || j == size / 2 - 1)) {
                board[i][j] = ' ';
            } else {
                do {
                    letterIndex = rand() % numLetters;
                } while (count[letterIndex] >= numEach + (extra > 0 ? 1 : 0));

                board[i][j] = letters[letterIndex];
                count[letterIndex]++;
                if (count[letterIndex] == numEach + 1) {
                    extra--;
                }
            }
        }
    }
    free(count);
}


///hamlenin kurallara uygun olup olmadiginin kontrolu
int isValidMove(char** board, int size, int x1, int y1, int x2, int y2, int* captureX, int* captureY) {
    if (x1 < 0 || x1 >= size || y1 < 0 || y1 >= size || x2 < 0 || x2 >= size || y2 < 0 || y2 >= size) 
        return 0;
    
    if (board[x2][y2] != ' ') 
        return 0;

    if (!(x1 == x2 || y1 == y2)) 
        return 0;

    if (x1 == x2) {
        if (abs(y1 - y2) != 2)
            return 0;
        
        *captureX = x1;
        *captureY = (y1 + y2) / 2;
        if (board[*captureX][*captureY] == ' ')
            return 0;
    }
    else {
        if (abs(x1 - x2) != 2)
            return 0;
        
        *captureX = (x1 + x2) / 2;
        *captureY = y1;
        if (board[*captureX][*captureY] == ' ')
            return 0;
    }

    return 1;
}


///tasi alir ve puani arttirir
void captureSkipper(char** board, Oyuncu* oyuncu, char skipper) {
    switch (skipper) {
        case 'A': oyuncu->a++; break;
        case 'B': oyuncu->b++; break;
        case 'C': oyuncu->c++; break;
        case 'D': oyuncu->d++; break;
        case 'E': oyuncu->e++; break;
    }
    oyuncu->score = oyuncu->score+ 10 ;
}
/// oyunu dosyaya yazdirmak icin txt tipinde yazar
void saveGame(char** board, int size, Oyuncu oyuncu1, Oyuncu oyuncu2, int turn,int vsComputer) {
    int i, j;
    FILE* file = fopen("saved_game.txt", "w");
    if (file == NULL) {
        printf("Dosya acilamadi.");
        exit(1);
    }

    fprintf(file, "%d\n", size);
    fprintf(file, "%d\n", turn);
    fprintf(file, "%d\n", vsComputer);

    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            fprintf(file, "%c ", board[i][j] == ' ' ? '-' : board[i][j]);
        }
        fprintf(file, "\n");
    }
    

    fprintf(file, "%d %d %d %d %d %d %d %d %d %d %d %d %d %d\n", oyuncu1.a, oyuncu1.b, oyuncu1.c, oyuncu1.d, oyuncu1.e, oyuncu1.sum,oyuncu1.score,
            oyuncu2.a, oyuncu2.b, oyuncu2.c, oyuncu2.d, oyuncu2.e, oyuncu2.sum,oyuncu2.score);

    fclose(file);
}
///txt tipinde kayit dsoyayisini yuklemek icin
void loadGame(char*** board, int* size, Oyuncu* oyuncu1, Oyuncu* oyuncu2, int* turn,int* vsComputer) {
    int i, j;
    FILE* file = fopen("saved_game.txt", "r");
    if (file == NULL) {
        printf("Kaydedilmis oyun bulunamadi.");
        exit(1);
    }

    fscanf(file, "%d", size);
    fscanf(file, "%d", turn);
    fscanf(file, "%d", vsComputer);
    *board = createBoard(*size);

    for (i = 0; i < *size; i++) {
        for (j = 0; j < *size; j++) {
            char ch;
            fscanf(file, " %c", &ch);
            (*board)[i][j] = (ch == '-') ? ' ' : ch; // 'dosyadan bosluk okumak sikintili oldugu icin dosyaya - olarak yazdiriyorum tekrar yuklerken - leri bosluk yapiyor
        }
     
    }
    fscanf(file, "%d %d %d %d %d %d %d %d %d %d %d %d %d %d",
        &((*oyuncu1).a), &((*oyuncu1).b), &((*oyuncu1).c), &((*oyuncu1).d), &((*oyuncu1).e), &((*oyuncu1).sum),&((*oyuncu1).score),
        &((*oyuncu2).a), &((*oyuncu2).b), &((*oyuncu2).c), &((*oyuncu2).d), &((*oyuncu2).e), &((*oyuncu2).sum),&((*oyuncu2).score));

    fclose(file);
}
//undo reno icin
void copyBoard(char*** dest, char** src, int size) {
    int i, j;
    *dest = createBoard(size);
    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            (*dest)[i][j] = src[i][j];
        }
    }
}
/// undo reno icin
void revertMove(char*** board, char*** prevBoard, int size) {
    char** temp = *board;
    *board = *prevBoard;
    *prevBoard = temp;
}
/// undo reno icin
void copyPlayer(Oyuncu* dest, Oyuncu* src) {
    dest->a = src->a;
    dest->b = src->b;
    dest->c = src->c;
    dest->d = src->d;
    dest->e = src->e;
    dest->sum = src->sum;
    dest->score = src->score;
}
/// oyun sonu kontolu icin hamle kalip kalmadiginin kontrolu
int checkForValidMoves(char** board, int size) {
    int x, y, dx, dy, captureX, captureY,i;
    int directions[4][2] = { {0, 2}, {0, -2}, {2, 0}, {-2, 0} };

    for (x = 0; x < size; x++) {
        for (y = 0; y < size; y++) {
            if (board[x][y] != ' ') {
                for (i = 0; i < 4; i++) {
                    dx = directions[i][0];
                    dy = directions[i][1];
                    if (isValidMove(board, size, x, y, x + dx, y + dy, &captureX, &captureY)) {
                        return 1;
                    }
                }
            }
        }
    }

    return 0;
}
// 1 set tamamlandiginda puani 100 arttirir
void checkAndResetSkippers(Oyuncu* oyuncu) { 
    if (oyuncu->a >0 && oyuncu->b > 0 && oyuncu->c >0 && oyuncu->d >0 && oyuncu->e >0) {
        oyuncu->sum++;
        oyuncu->a--;
        oyuncu->b--;
        oyuncu->c--;
        oyuncu->d--;
        oyuncu->e--;
        oyuncu->score = oyuncu->score+100;
    }
}
//bir tasla hamle yaptikran sonra o tasla baska hamle yapilip yapilamayacaginin kontrolu
int checkAdditionalMove(char** board, int size, int x, int y) { 
    int dx, dy, captureX, captureY,i;
    int directions[4][2] = { {0, 2}, {0, -2}, {2, 0}, {-2, 0} };

    for (i = 0; i < 4; i++) {
        dx = directions[i][0];
        dy = directions[i][1];
        if (isValidMove(board, size, x, y, x + dx, y + dy, &captureX, &captureY)) {
            return 1;
        }
    }
    return 0;
}

int main() {
    int size, control,prevcontinueMove,continueMove;
    char** board;
    char** prevBoard;
    Oyuncu oyuncu1 = {0, 0, 0, 0, 0, 0};
    Oyuncu oyuncu2 = {0, 0, 0, 0, 0, 0};
    Oyuncu oyuncu1Backup, oyuncu2Backup,oyuncu1Redo,oyuncu2Redo;
    int x1, y1, x2, y2, captureX, captureY, prevX, prevY;
    int turn = 0; // 0: oyuncu1, 1: oyuncu2
    char choice;
    int canUndo = 1;
    int canRedo = 1;
    int vsComputer = 0; // 1: bilgisayara karþý oynama modu
    prevX = 0;
    prevY=0;

    printf("2 kisilik mod icin 'd'ye\nkaydedilmis oyunu yuklemek icin 'y'ye \nbilgisayara karsi oynamak icin 'b'ye basin: \n ");
    scanf(" %c", &choice);

    if (choice == 'd') {
        printf("Tahta boyutunu girin (cift sayi): ");
        scanf("%d", &size);

        if (size % 2 != 0 || size > 20 || size < 6) {
            printf("Boyut cift sayi olmalidir ve 20 ile 6 arasinda olmalidir.\n");
            return 1;
        }

        board = createBoard(size);
        prevBoard = createBoard(size);
        fillBoard(board, size);
    } else if (choice == 'y') {
        loadGame(&board, &size, &oyuncu1, &oyuncu2, &turn, &vsComputer);
        copyBoard(&prevBoard, board, size);
        copyPlayer(&oyuncu1Backup, &oyuncu1);
        copyPlayer(&oyuncu2Backup, &oyuncu2);
        
    } else if (choice == 'b') {
        vsComputer = 1;
        printf("Tahta boyutunu girin (cift sayi): ");
        scanf("%d", &size);

        if (size % 2 != 0 || size > 20 || size < 6) {
            printf("Boyut cift sayi olmalidir ve 20 ile 6 arasinda olmalidir.\n");
            return 1;
        }

        board = createBoard(size);
        prevBoard = createBoard(size);
        fillBoard(board, size);
    } else {
        printf("Gecersiz secim. Programdan cikiliyor.");
        return 1;
    }

    continueMove = 0;  
    control = 1;
    prevcontinueMove = 0;/// undo renoda art arda hamlede sorun cikmamasi icin

    while (control) {
        system("cls");
        printBoard(board, size);
        printf("Oyuncu1:\nA:%d\nB:%d\nC:%d\nD:%d\nE:%d\nS:%d\nscore: %d\n", oyuncu1.a, oyuncu1.b, oyuncu1.c, oyuncu1.d, oyuncu1.e, oyuncu1.sum, oyuncu1.score);
        printf("Oyuncu2:\nA:%d\nB:%d\nC:%d\nD:%d\nE:%d\nS:%d\nscore: %d\n", oyuncu2.a, oyuncu2.b, oyuncu2.c, oyuncu2.d, oyuncu2.e, oyuncu2.sum, oyuncu2.score);

        if (continueMove) {
            printf("Oyuncu %d'nin tekrar sira (x2 y2) veya geri almak icin -1 -1,ileri almak icin -2 -2, gecmek icin -3 -3\n", turn + 1);
        } else {
            printf("Oyuncu %d'nin sira (x1 y1 x2 y2) (Geri almak icin -1 -1 -1 -1, Ileri almak icin -2 -2 -2 -2): \n", turn + 1);
        }

        if (continueMove && ((vsComputer == 1 && turn == 0) || vsComputer == 0)){/// oyuncunun art arda hamlesi 
            scanf("%d %d", &x2, &y2);
            if (x2 == -3 && y2 == -3) {
            	x1 = 0;
            	y1 = 0;
                turn = 1 - turn;
                continueMove = 0;
                canUndo = 1;
                canRedo = 1;
                printf("PASS\n");
            }else if(x2 == -1 && y2 == -1){
            	x1 = -1;
            	y1 = -1;
            	canUndo = 1;
            	canRedo = 1;
            	turn = 1 -turn;
			}
			else if(x2 == -2 && y2 == -2 ){
				x1 = -2;
				y1 = -2;
				continueMove = 0;
				prevcontinueMove = 0;
				canUndo = 0;
				canRedo = 1;
			}else {
                x1 = prevX;
                y1 = prevY;
            }
        } 
        else if(continueMove == 1 && vsComputer && turn == 1){/// bilgisayar art arda hamlesi
        	Move bestMove = findBestMove(board, size, oyuncu1, oyuncu2, turn,prevX,prevY);
        	x1 = prevX;
        	y1 = prevY;
        	x2 = bestMove.x2;
        	y2 = bestMove.y2;
        	printf("Bilgisayar hamlesi: (%d %d %d %d)\n", x1, y1, x2, y2);
		}
		else if(vsComputer && turn == 1 && continueMove == 0){
			printf("Bilgisayarin sirasi.\nGeri almak icin -1 -1 -1 -1 devam etmek icin herhangi 4 sayi giriniz\n");
			scanf("%d %d %d %d", &x1, &y1, &x2, &y2);
			if(x1 == -1 && x2 == -1 && y1 == -1 && y2 == -1){
				printf("Geri aliniyor \n");
			}else{
				Move bestMove = findBestMove(board, size, oyuncu1, oyuncu2, turn,-1,-1);
				x1 = bestMove.x1;
				y1 = bestMove.y1;
				x2 = bestMove.x2;
				y2 = bestMove.y2;
				printf("Bilgisayar hamlesi: (%d %d %d %d)\n", x1, y1, x2, y2);
				prevX = x2;
				prevY= y2;
			}
		
        
        }else {
            scanf("%d %d %d %d", &x1, &y1, &x2, &y2);
            }

        if (x1 == -1 && y1 == -1 && x2 == -1 && y2 == -1 && canUndo) { // Geri alma
            revertMove(&board, &prevBoard, size);
            continueMove = prevcontinueMove;
            copyPlayer(&oyuncu1Redo,&oyuncu1);
            copyPlayer(&oyuncu2Redo,&oyuncu2);
            copyPlayer(&oyuncu1, &oyuncu1Backup);
            copyPlayer(&oyuncu2, &oyuncu2Backup);
            turn = 1 - turn;
            canUndo = 0;
            canRedo = 1;
        } else if (x1 == -2 && y1 == -2 && x2 == -2 && y2 == -2 && canRedo) { // Ileri alma
            revertMove(&board, &prevBoard, size);
            copyPlayer(&oyuncu1,&oyuncu1Redo);
            copyPlayer(&oyuncu2,&oyuncu2Redo);
            turn = 1 - turn;
            continueMove = prevcontinueMove;
            canRedo = 0;
            canUndo = 1;
        } else {
            if (isValidMove(board, size, x1, y1, x2, y2, &captureX, &captureY)) {/// tas yeme
                copyBoard(&prevBoard, board, size);
                prevcontinueMove = continueMove;
                copyPlayer(&oyuncu1Backup, &oyuncu1);
                copyPlayer(&oyuncu2Backup, &oyuncu2);
                char skipper = board[captureX][captureY];
                board[x2][y2] = board[x1][y1];
                board[x1][y1] = ' ';
                board[captureX][captureY] = ' ';

                if (turn == 0) {
                    captureSkipper(board, &oyuncu1, skipper);
                } else {
                    captureSkipper(board, &oyuncu2, skipper);
                }

                checkAndResetSkippers(turn == 0 ? &oyuncu1 : &oyuncu2);

                if (checkAdditionalMove(board, size, x2, y2)) {
                    continueMove = 1;
                    prevX = x2;
                    prevY = y2;
                } else {
                    turn = 1 - turn;
                    continueMove = 0;
                }
                canUndo = 1;
                canRedo = 1;
            } else {
                
                printf("%d %d %d %d",x1,y1,x2,y2);
                printf("Gecersiz hamle, tekrar deneyin.\n");
            }
        }

        if (!checkForValidMoves(board, size)) {/// oyun sonu kontrolu
            printf("Oyun bitti! \n");
            printf("Oyuncu1 Puan: %d\n", oyuncu1.score);
            printf("Oyuncu2 Puan: %d\n", oyuncu2.score);
            if (oyuncu1.score > oyuncu2.score) {
                printf("Oyuncu 1 kazandi!\n");
            } else if (oyuncu2.score > oyuncu1.score) {
                printf("Oyuncu 2 kazandi!\n");
            } else {
                printf("Berabere!");
            }
            printf("cikmak icim Herhangi Bir tusa basiniz");
            scanf("%c",&choice);
            control = 0; 
        }

        if (control) {
            printf("Oyuna devam etmek icin 'd'ye, oyundan cikmak icin 'c'ye basin: ");
            scanf(" %c", &choice);
            if (choice == 'c') {
                saveGame(board, size, oyuncu1, oyuncu2, turn,vsComputer);
                control = 0;
            }
        }
    
    }
    freeBoard(board, size);
    freeBoard(prevBoard, size);
    return 0;
}

