#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

#define MAX_LEN 256
#define MAX_CHAR_SCAN "%255s"
#define GARBAGE_CAP 100 // має бути рівна CLIENT_NUMBER
#define CLIENT_NUMBER 100
#define BRIGADE_NUMBER 100
#define ORDER_NUMBER 10000

typedef struct TClient
{
    int id;
    char name [MAX_LEN]; //ім'я
    char phone [MAX_LEN];
    int first_s;
} Client;

typedef struct TStack
{
    int arr[GARBAGE_CAP];
    int top; // індекс першого елемента без значення
} Stack;

typedef struct TOrder
{
    int client_id;
    int brigade_id;
    char name[MAX_LEN]; // назва
    char final_date[MAX_LEN];
    char info[MAX_LEN];
    int next_addr;
} Order;

Client get_m (FILE *f, int id, int *itable);

// якщо файл fl пустий, то потрібно заповнити стек сміття і записати його в файл
void preproc(FILE *f, Stack *garbage)
{
    fseek(f, 0L, SEEK_SET);
    for (int i = 1; i < GARBAGE_CAP; i++)
        garbage->arr[i] = -1;
    garbage->arr[0] = 0;
    garbage->top = 1;
    fwrite(&(garbage->top), sizeof(int), 1, f);
    fwrite(garbage->arr, sizeof(int), GARBAGE_CAP, f);
    fseek(f, 0L, SEEK_SET);
}

void insert_m (FILE *f, Stack *garbage, int* itable)
{
    Client one;
    int p = 1;
    while(p)
    {
        one.id = 0;
        memset(one.name, 0, MAX_LEN);
        memset(one.phone, 0, MAX_LEN);

        printf("Введіть код клієнта\n");
        if (scanf("%d", &(one.id)) == 0)
        {
            printf("Формат коду неправильний, спробуйте знову\nВвід буде розпочато знову\n");
            fflush(stdin);
            continue;
        }
        if ((one.id < 0) || (one.id >= CLIENT_NUMBER))
        {
            printf("Введіть число в проміжку від 0 до %d включно\nВвід буде розпочато знову\n", CLIENT_NUMBER - 1);
            continue;
        }
        if (itable[one.id] != -1)
        {
            printf("Клієнт з таким кодом вже існує. Введіть новий код\nВвід буде розпочато знову\n");
            continue;
        }

        printf("Введіть назву клієнта (латиницею)\n");
        if (scanf(MAX_CHAR_SCAN, one.name) == 0)
        {
            printf("Формат імені неправильний, спробуйте знову\nВвід буде розпочато знову\n");
            fflush(stdin);
            continue;
        }
        printf("Введіть номер телефону клієнта\n");
        if (scanf(MAX_CHAR_SCAN, one.phone) == 0)
        {
            printf("Формат телефону неправильний, спробуйте знову\nВвід буде розпочато знову");
            fflush(stdin);
            continue;
        }
        p = 0;
    }
    printf("Клієнт успішно введений\n\n");
    one.first_s = -1;
    itable[one.id] = garbage->arr[garbage->top - 1];
    fseek(f, (long) sizeof(int)+sizeof(int)*GARBAGE_CAP + garbage->arr[garbage->top - 1]*(sizeof(Client)), SEEK_SET);
    fwrite(&one, sizeof(Client), 1, f);
    garbage->top--;
    if (garbage->top == 0)
    {
        fseek(f, 0L, SEEK_END);
        int capacity = (ftell(f) - ((long) sizeof(int) + sizeof(int)*GARBAGE_CAP));
        int number = capacity/sizeof(Client);
        garbage->top = 1;
        garbage->arr[0] = number;
    }
    fseek(f, 0L, SEEK_SET);
}

void insert_s (FILE *mf, FILE *sf, Stack *garbage, int *itable)
{
    Order ord;
    int p = 1;
    while (p)
    {
        ord.client_id = 0;
        ord.brigade_id = 0;
        memset(ord.name, 0, MAX_LEN);
        memset(ord.info, 0, MAX_LEN);
        memset(ord.final_date, 0, MAX_LEN);
        ord.next_addr = -1;
        int r = 1;
        while(r)
        {
            printf("Введіть код клієнта\n");
            if (scanf("%d", &(ord.client_id)) == 0)
            {
                printf("Формат коду неправильний, спробуйте знову\nВвід буде розпочато знову\n\n");
                fflush(stdin);
                continue;
            }
            if ((ord.client_id < 0) || (ord.client_id >= CLIENT_NUMBER))
            {
                printf("Введіть число в проміжку від 0 до %d включно\nВвід буде розпочато знову\n", CLIENT_NUMBER - 1);
                continue;
            }
            if (itable[ord.client_id] == -1)
            {
                printf("Клієнта з таким кодом не існує. Введіть новий код\nВвід буде розпочато знову\n");
                continue;
            }
            r = 0;
        }

        //Client one = get_m(f, ord.client_id, itable);
        int t = 1;
        while(t)
        {
            printf("Введіть код бригади\n");
            if (scanf("%d", &(ord.brigade_id)) == 0)
            {
                printf("Формат коду неправильний, спробуйте знову\nВвід буде розпочато знову\n");
                fflush(stdin);
                continue;
            }
            if ((ord.brigade_id < 0) || (ord.brigade_id >= BRIGADE_NUMBER))
            {
                printf("Введіть число в проміжку від 0 до %d включно\nВвід буде розпочато знову\n", BRIGADE_NUMBER - 1);
                fflush(stdin);
                continue;
            }
            t = 0;
        }
        int y = 1;
        while(y)
        {
            printf("Введіть назву замовлення (латиницею)\n");
            if (scanf(MAX_CHAR_SCAN, ord.name) == 0)
            {
                printf("Формат імені неправильний, спробуйте знову\nВвід буде розпочато знову\n");
                fflush(stdin);
                continue;
            }
            y = 0;
        }
        int e = 1;
        while (e)
        {
            printf("Введіть кінцеву дату виконання замовлення\n");
            if (scanf(MAX_CHAR_SCAN, ord.final_date) == 0)
            {
                printf("Формат дати неправильний, спробуйте знову\nВвід буде розпочато знову\n");
                fflush(stdin);
                continue;
            }
            e = 0;
        }
        e = 1;
        while (e)
        {
            printf("Введіть іншу інформацію про замовлення\n");
            if (scanf(MAX_CHAR_SCAN, ord.info) == 0)
            {
                printf("Формат тексту неправильний, спробуйте знову\nВвід буде розпочато знову\n");
                fflush(stdin);
                continue;
            }
            e = 0;
        }
        p = 0;
    }
    Client one;
    one = get_m(mf, ord.client_id, itable);
    ord.next_addr = -1;
    if (one.first_s != -1)
    {
        Order another;
        fseek(sf, (long) sizeof(int) + sizeof(int)*GARBAGE_CAP + one.first_s*sizeof(Order), SEEK_SET);
        fread(&another, sizeof(Order), 1, sf);
        while(another.next_addr != -1)
        {
            fseek(sf, (long) sizeof(int) + sizeof(int)*GARBAGE_CAP + another.next_addr*sizeof(Order), SEEK_SET);
            fread(&another, sizeof(Order), 1, sf);
        }
        another.next_addr = garbage->arr[garbage->top - 1];
        fseek(sf, (long) (-1)*sizeof(Order), SEEK_CUR);// сумнівно
        fwrite(&another, sizeof(Order), 1, sf);//

        fseek(sf, (long) sizeof(int) + sizeof(int)*GARBAGE_CAP + another.next_addr*sizeof(Order), SEEK_SET);
        fwrite(&ord, sizeof(Order), 1, sf);
    }
    else
    {
        one.first_s = garbage->arr[garbage->top - 1];
        fseek(mf, (long) sizeof(int) + sizeof(int)*GARBAGE_CAP + itable[one.id]*sizeof(Client), SEEK_SET);
        fwrite(&one, sizeof(Client), 1, mf);

        fseek(sf, (long) sizeof(int) + sizeof(int)*GARBAGE_CAP + one.first_s*sizeof(Order), SEEK_SET);
        fwrite(&ord, sizeof(Order), 1, sf);
    }

    garbage->top--;
    if (garbage->top == 0)
    {
        fseek(sf, 0L, SEEK_END);
        garbage->top = 1;
        garbage->arr[0] = (ftell(sf) - ((long) sizeof(int) + sizeof(int)*GARBAGE_CAP))/sizeof(Order);
    }
    fseek(sf, 0L, SEEK_SET);
    fseek(mf, 0L, SEEK_SET);
    printf("Замовлення успішно введено\n\n");
}

void ut_m (FILE *f, Stack *garbage, int *itable)
{
    printf("%d\n", garbage->top); //службова штука
    for (int i = 0; i < GARBAGE_CAP; i++)//
        printf("%d ", garbage->arr[i]);//
    printf("\nitable:\n");
    for (int i = 0; i < CLIENT_NUMBER; i++)
        printf("%d ", itable[i]);
    printf("\n");// кінець службової штуки

    fseek(f, (long) sizeof(int) + sizeof(int)*GARBAGE_CAP, SEEK_SET);
    int l = ftell(f);
    int count = 0;
    while(!feof(f))
    {
        Client one;
        l = ftell(f);
        fseek(f, 0L, SEEK_END);
        int end = ftell(f);
        if (l == end)
             break;
        fseek(f, (long) l, SEEK_SET);
        fread(&one, sizeof(Client), 1, f);
        l = ftell(f);
        if (itable[one.id] == -1)
            continue;
        count++;
        print_client(&one, count);
    }
    if (count == 0)
        printf("В базі поки відсутні клієнти\n");
    fseek(f, 0L, SEEK_SET);
    printf("\n");
}

void ut_s (FILE *mf, FILE *sf, Stack *garbage, int *itable, int client_id)
{
    printf("%d\n", garbage->top); //службова штука
    for (int i = 0; i < GARBAGE_CAP; i++)//
        printf("%d ", garbage->arr[i]);//
    printf("\n");// кінець службової штуки

    Client one = get_m(mf, client_id, itable);
    if (one.first_s == -1)
    {
        printf("У цього клієнта немає замовлень\n\n");
        return;
    }
    fseek(sf, (long) sizeof(int) + sizeof(int)*GARBAGE_CAP + one.first_s*sizeof(Order), SEEK_SET);
    Order another;
    int s = ftell(sf);
    fread(&another, sizeof(Order), 1, sf);
    s = ftell(sf);
    print_order(&another, 1);
    while (another.next_addr != -1)
    {
        fseek(sf, (long) sizeof(int) + sizeof(int)*GARBAGE_CAP + another.next_addr*sizeof(Order), SEEK_SET);
        s = ftell(sf);
        fread(&another, sizeof(Order), 1, sf);
        print_order(&another, 0);
    }
    fseek(sf, 0L, SEEK_SET);
    printf("\n");
}

int del_s(FILE *mf, FILE *sf, int client_id, Stack *sgarbage, int *itable)
{
    Client one;
    one = get_m(mf, client_id, itable);
    if (one.first_s == -1)
    {
        printf("У цього клієнта немає замовлень\n");
        return -1;
    }
    int br_id = 0;
    int t = 1;
    while(t)
    {
        br_id = 0;
        printf("Введіть код бригади\n");
        if (scanf("%d", &br_id) == 0)
        {
            printf("Формат коду неправильний, спробуйте знову\nВвід буде розпочато знову\n");
            fflush(stdin);
            continue;
        }
        if ((br_id < 0) || (br_id >= BRIGADE_NUMBER))
        {
            printf("Введіть число в проміжку від 0 до %d включно\nВвід буде розпочато знову\n", BRIGADE_NUMBER - 1);
            continue;
        }
        t = 0;
    }
    int flag = 0;
    Order res;
    fseek(sf, 0L, SEEK_SET);
    fseek(sf, sizeof(int) + sizeof(int)*GARBAGE_CAP + one.first_s*sizeof(Order),SEEK_SET);
    int s = ftell(sf);
    fread(&res, sizeof(Order), 1, sf);
    s = ftell(sf);
    if (res.brigade_id == br_id)
    {
        sgarbage->arr[sgarbage->top] = one.first_s;
        one.first_s = res.next_addr;
        fseek(mf, (long)sizeof(int) + sizeof(int)*GARBAGE_CAP + itable[one.id]*sizeof(Client), SEEK_SET);
        fwrite(&one, sizeof(Client), 1, mf);
        sgarbage->top++;
        flag = 1;
    }
    //впродовж циклу prev2 вказує на prev, prev - на res, res - на наступний
    int prev2 = one.first_s;
    Order prev = res;
    while((res.next_addr != -1) && (flag == 0))
    {
        prev = res;
        fseek(sf, (long) sizeof(int) + sizeof(int)*GARBAGE_CAP + prev.next_addr*sizeof(Order), SEEK_SET);
        s = ftell(sf);
        fread(&res, sizeof(Order), 1, sf);
        s = ftell(sf);
        if (res.brigade_id == br_id)
        {
            sgarbage->arr[sgarbage->top] = prev.next_addr;
            int s = fseek(sf, (long) sizeof(int) + sizeof(int)*GARBAGE_CAP + prev2*sizeof(Order), SEEK_SET);
            prev.next_addr = res.next_addr;
            s = fwrite(&prev, sizeof(Order), 1, sf);
            sgarbage->top++;
            flag = 1;
        }
        prev2 = prev.next_addr;
    }
    fseek(mf, 0L, SEEK_SET);
    fseek(sf, 0L, SEEK_SET);
    if (flag == 0)
    {
        printf("Такого замовлення не існує\n\n");
        return 0;
    }
    else
    {
        printf("Замовлення успішно видалено\n\n");
        return 1;
    }
}

void del_m (FILE *mf, FILE *sf, int id, int *itable, Stack *garbage, Stack *sgarbage)
{
    Client one = get_m(mf, id, itable);
    if (one.first_s != -1)
    {
        Order res;
        fseek(sf, 0L, SEEK_SET);
        fseek(sf, sizeof(int) + sizeof(int)*GARBAGE_CAP + one.first_s*sizeof(Order),SEEK_SET);
        fread(&res, sizeof(Order), 1, sf);
        sgarbage->arr[sgarbage->top] = one.first_s;
        sgarbage->top++;
        while(res.next_addr != -1)
        {
            sgarbage->arr[sgarbage->top] = res.next_addr;
            sgarbage->top++;
            fseek(sf, (long) sizeof(int) + sizeof(int)*GARBAGE_CAP + res.next_addr*sizeof(Order), SEEK_SET);
            fread(&res, sizeof(Order), 1, sf);
        }
    }
    garbage->arr[garbage->top] = itable[id];
    garbage->top++;
    itable[id] = -1;
    fseek(mf, 0L, SEEK_SET);
    fseek(sf, 0L, SEEK_SET);
}

int get_s (FILE *mf, FILE *sf, int client_id, int *itable, Order *res)
{
    Client one;
    one = get_m(mf, client_id, itable);
    if (one.first_s == -1)
    {
        printf("У цього клієнта немає замовлень\n");
        return -1;
    }
    int br_id = 0;
    int t = 1;
    while(t)
    {
        br_id = 0;
        printf("Введіть код бригади\n");
        if (scanf("%d", &br_id) == 0)
        {
            printf("Формат коду неправильний, спробуйте знову\nВвід буде розпочато знову\n");
            fflush(stdin);
            continue;
        }
        if ((br_id < 0) || (br_id >= BRIGADE_NUMBER))
        {
            printf("Введіть число в проміжку від 0 до %d включно\nВвід буде розпочато знову\n", BRIGADE_NUMBER - 1);
            continue;
        }
        t = 0;
    }
    int flag = 0;
    fseek(sf, 0L, SEEK_SET);
    fseek(sf, sizeof(int) + sizeof(GARBAGE_CAP) + one.first_s*sizeof(Order),SEEK_SET);
    fread(res, sizeof(Order), 1, sf);
    if (res->brigade_id == br_id)
        flag = 1;
    while((res->next_addr != -1) && (flag == 0))
    {
        fseek(sf, (long) sizeof(int) + sizeof(int)*GARBAGE_CAP + res->next_addr*sizeof(Order), SEEK_SET);
        fread(res, sizeof(Order), 1, sf);
        if (res->brigade_id == br_id)
            flag = 1;
    }
    fseek(mf, 0L, SEEK_SET);
    fseek(sf, 0L, SEEK_SET);
    if (flag == 0)
    {
        printf("Такого замовлення не існує\n");
        return 0;
    }
    else
    {
        return 1;
    }
}

Client get_m (FILE *f, int id, int *itable)
{
    Client res;
    int row = itable[id];
    fseek(f, (long) sizeof(int) + sizeof(int)*GARBAGE_CAP + row*sizeof(Client), SEEK_SET);
    fread(&res, sizeof(Client), 1, f);
    fseek(f, 0L, SEEK_SET);
    return res;
}

void upd_m (FILE *f, Client *one, int *itable)
{
    int row = itable[one->id];
    fseek(f, (long)sizeof(int) + sizeof(int)*GARBAGE_CAP + row*sizeof(Client), SEEK_SET);
    fwrite(one, sizeof(Client), 1, f);
    fseek(f, 0L, SEEK_SET);
}

void upd_s (FILE *mf, FILE *sf, Order *another, int client_id, int itable)
{
    Client one;
    one = get_m(mf, client_id, itable);
    if (one.first_s == -1)
    {
        printf("У цього клієнта немає замовлень\n\n");
        return -1;
    }

    int flag = 0;
    Order res;
    fseek(sf, sizeof(int) + sizeof(int)*GARBAGE_CAP + one.first_s*sizeof(Order),SEEK_SET);
    fread(&res, sizeof(Order), 1, sf);
    if (res.brigade_id == another->brigade_id)
    {
        fseek(sf, (long) sizeof(int) + sizeof(int)*GARBAGE_CAP + one.first_s*sizeof(Order), SEEK_SET);
        fwrite(another, sizeof(Order), 1, sf);
        flag = 1;
    }
    while((res.next_addr != -1) && (flag == 0))
    {
        fseek(sf, (long) sizeof(int) + sizeof(int)*GARBAGE_CAP + res.next_addr*sizeof(Order), SEEK_SET);
        fread(&res, sizeof(Order), 1, sf);
        if (res.brigade_id == another->brigade_id)
        {
            fseek(sf, (long) (-1)*sizeof(Order), SEEK_CUR);// сумнівно
            fwrite(another, sizeof(Order), 1, sf);
            flag = 1;
        }
    }
    fseek(mf, 0L, SEEK_SET);
    fseek(sf, 0L, SEEK_SET);
    if (flag == 0)
    {
        printf("Такого замовлення не існує\n\n");
        return 0;
    }
    else
    {
        printf("Зміни виконано успішно\n\n");
        return 1;
    }
}
int scan_code(FILE *f, int *itable, Stack *garbage)
{
    int p = 1;
    while(p)
    {
        printf("Введіть код клієнта\n");
        printf("Щоб вивести всіх клієнтів, введіть команду ut-m\n");
        printf("Щоб скасувати команду, введіть cancel\n");
        char s[MAX_LEN];
        scanf(MAX_CHAR_SCAN, s);
        // введений рядок - або число, або 0 (особливий випадок), або рядок
        int id = 0;
        if(s[0] == '0')
        {
            if(itable[0] == -1)
            {
                printf("Такого елемента не існує\nВвід буде розпочато заново\n\n");
                continue;
            }
            printf("Введений код: %d\n", 0);
            return 0;
        }
        else if ((id = atoi(s)) != 0)
        {
            if(id < 0)
            {
                printf("Код повинен бути додатнім\nВвід буде розпочато заново\n\n");
                continue;
            }
            else if(itable[id] == -1)
            {
                printf("Такого елемента не існує\nВвід буде розпочато заново\n\n");
                continue;
            }
            else
            {
                printf("Введений код: %d\n", id);
                return id;
            }
        }
        else if (strcmp(s, "cancel") == 0)
        {
            printf("Команду скасовано\n\n");
            return -1;
        }
        else if(strcmp(s, "ut-m") == 0)
        {
            printf("Всі клієнти:\n");
            ut_m(f, garbage, itable);
            printf("\n");
            continue;
        }
        else
        {
            printf("Такої команди не існує, спробуйте знову");
            printf("\n\n");
            continue;
        }
        p = 0;// воно не повинно дійти до цього рядка
    }
    return -1;
}

void print_client(Client *one, int is_first)
{
    if (is_first == 1)
        printf("%-16s %-16s %-16s %-16s\n", "Код", "Назва", "Телефон", "Номер першого запису");
    printf("%-16d %-16s %-16s %-16d\n", one->id, one->name, one->phone, one->first_s);
}

void print_order(Order *another, int is_first)
{
    if (is_first == 1)
        printf("%-16s %-16s %-16s %-16s %-16s %-16s\n", "Код клієнта", "Код бригади","Назва замовлення",
                                                      "Інформація", "Кінцева дата", "Адреса наступного");
    printf("%-16d %-16d %-16s %-16s %-16s %-16d\n", another->client_id, another->brigade_id,
                another->name, another->info, another->final_date, another->next_addr);
}
void scan_garbage(FILE *f, Stack *garbage)
{
    fseek(f, 0L, SEEK_END);
    if (ftell(f) == 0)
        preproc(f, garbage);// не потрібно ставити зчитування знову на початок, оскільки файл пустий
    else
    {
        fseek(f, 0L, SEEK_SET);
        fread(&(garbage->top), sizeof(int), 1, f);
        fread(garbage->arr, sizeof(int), GARBAGE_CAP, f);
    }
    fseek(f, 0L, SEEK_SET);
}

void scan_itable(FILE *f, int *itable)
{
    for (int i = 0; i < CLIENT_NUMBER; i++)
    {
        itable[i] = -1;
    }

    fseek(f, 0L, SEEK_END);
    int end = ftell(f);

    if (end != 0)
    {
        fseek(f, 0L, SEEK_SET);
        while (!feof(f))
        {
            int id = 0;
            int row = 0;
            if (end == ftell(f))
                break;
            if(fread(&id, sizeof(int), 1, f) == 0)
                printf("smth wrong\n");
            if(fread(&row, sizeof(int), 1, f) == 0)
                printf("smth wrong 2\n");
            itable[id] = row;
        }
    }
    fseek(f, 0L, SEEK_SET);
}

void print_help()
{
    printf("Список команд:\n");
    printf("get-m - знайти заданого клієнта\n");
    printf("get-s - для заданого клієнта знайти потрібне замовлення\n");
    printf("del-m - видалити заданого клієнта\n");
    printf("del-s - для заданого клієнта видалити введене замовлення\n");
    printf("update-m - оновити значення потрібного поля клієнта\n");
    printf("update-s - для заданого клієнта оновити значення потрібного замовлення\n");
    printf("insert-m - внести новго клієнта\n");
    printf("insert-s - для заданого клієнта внести нове замовлення\n");
    printf("ut-m - вивести всіх клієнтів\n");
    printf("ut-s - вивести всі замовлення потрібного клієнта\n");
    printf("help - виклик довідки\n");
    printf("exit - завершити роботу з програмою\n\n");
}

int main()
{
    char getm [] = "get-m";//
    char gets [] = "get-s";
    char delm [] = "del-m";//
    char dels [] = "del-s";
    char updm [] = "update-m";//
    char upds [] = "update-s";
    char insertm [] = "insert-m";//
    char inserts [] = "insert-s";
    char utm [] = "ut-m";//
    char uts [] = "ut-s";

    setlocale(LC_ALL, "" );
    char cmd[MAX_LEN];

    printf("Програма для керування БД\n");
    print_help();


    //Всі файли повинні бути створені до запуску програми

    //Зчитування S.fl
    Stack garbage;
    FILE *fl, *ind;
    fl = fopen("S.fl", "r+b");
    scan_garbage(fl, &garbage);

    //Зчитування S.ind
    ind = fopen("S.ind", "r+b");
    int itable[CLIENT_NUMBER];
    scan_itable(ind, itable);

    // Зчитування Sp.fl
    Stack sgarbage;
    FILE *sfl;
    sfl = fopen("Sp.fl", "r+b");
    scan_garbage(sfl, &sgarbage);

    // Головний цикл
        while(scanf(MAX_CHAR_SCAN, cmd))
    {
        if (!strcmp(cmd, getm))
        {
            int id = 0;
            if ((id = scan_code(fl, itable, &garbage)) != -1)
            {
                Client toprint;
                toprint = get_m(fl, id, itable);
                print_client(&toprint, 1);
            }
        }
        else if (!strcmp(cmd, gets))
        {
            int id = 0;
            if ((id = scan_code(fl, itable, &garbage)) != -1)
            {
                Order res;
                int flag = get_s(fl, sfl, id, itable, &res);
                if (flag == 1)
                {
                    printf("Потрібне замовлення: \n");
                    print_order(&res, 1);
                }
            }
        }
        else if (!strcmp(cmd, delm))
        {
            int id = 0;
            if ((id = scan_code(fl, itable, &garbage)) != -1)
            {
                del_m(fl, sfl, id, itable, &garbage, &sgarbage);
                printf("Клієнта з номером %d видалено успішно\n\n", id);
            }
        }
        else if (!strcmp(cmd, dels))
        {
            int id = scan_code(fl, itable, &garbage);
            if (id != -1)
            {
                del_s(fl, sfl, id, &sgarbage, itable);
            }
        }
        else if (!strcmp(cmd, updm))
            {
                int id = 0;
                if((id = scan_code(fl, itable, &garbage)) != -1)
                {
                    printf("Введіть код поля, яке ви хочете змінити\n");
                    printf("1 - Назву\n2 - Телефон\n");
                    Client tochange;
                    tochange = get_m(fl, id, itable);
                    int attribute = 0;
                    scanf("%d", &attribute);
                    if (attribute == 1)
                    {
                        printf("Введіть нову назву клієнта (латиницею)\n");
                        scanf(MAX_CHAR_SCAN, tochange.name);
                        upd_m(fl, &tochange, itable);
                        printf("Клієнта номер %d успішно оновлено\n\n", tochange.id);
                    }
                    else if (attribute == 2)
                    {
                        printf("Введіть новий номер телефону клієнта\n");
                        scanf(MAX_CHAR_SCAN, tochange.phone);
                        upd_m(fl, &tochange, itable);
                        printf("Клієнта номер %d успішно оновлено\n\n", tochange.id);
                    }
                    else
                    {
                        printf("Введено недопустимий код атрибуту\nКоманда буде закінчена\n");
                        fflush(stdin);
                    }
                }
            }
        else if (!strcmp(cmd, upds))
        {
            int id = scan_code(fl, itable, &garbage);
            if (id != -1)
            {
                Order tochange;
                if(get_s(fl, sfl, id, itable, &tochange) == 1)
                {
                    printf("Введіть код атрибуту, який ви хочете змінити\n");
                    printf("1 - Назва\n2 - Інфо\n3 - Кінцеву дату\n");
                    int att = 0;
                    scanf("%d", &att);
                    if (att == 1)
                    {
                        printf("Введіть нову назву замовлення (латиницею)\n");
                        scanf(MAX_CHAR_SCAN, tochange.name);
                        upd_s(fl, sfl, &tochange, id, itable);
                    }
                    else if (att == 2)
                    {
                        printf("Введіть нову інформацію про замовлення (латиницею)\n");
                        scanf(MAX_CHAR_SCAN, tochange.info);
                        upd_s(fl, sfl, &tochange, id, itable);
                    }
                    else if (att == 3)
                    {
                        printf("Введіть нову дату закінчення замовлення\n");
                        scanf(MAX_CHAR_SCAN, tochange.final_date);
                        upd_s(fl, sfl, &tochange, id, itable);
                    }
                    else
                    {
                        printf("Введено недопустимий код атрибуту\nКоманда буде закінчена\n");
                        fflush(stdin);
                    }
                }
            }
        }
        else if (!strcmp(cmd, insertm))
            insert_m(fl, &garbage, itable);
        else if (!strcmp(cmd, inserts))
            insert_s(fl, sfl, &sgarbage, itable);
        else if (!strcmp(cmd, utm))
            ut_m(fl, &garbage, itable);
        else if (!strcmp(cmd, uts))
        {
            int id = 0;
            id = scan_code(fl, itable, &garbage);
            if (id == -1);
            else
                ut_s(fl, sfl, &sgarbage, itable, id);
        }
        else if (!strcmp(cmd, "help"))
        {
            print_help();
        }
        else if (!strcmp(cmd, "exit"))
        {
            break;
            printf("Вихід\n");
        }
        else
            printf("Введеної команди не існує. Спробуйте ще раз.\nДля виклику допомоги введіть help\n\n");
    }

    fseek(sfl, 0L, SEEK_SET);
    fwrite(&(sgarbage.top), sizeof(int), 1, sfl);
    fwrite(sgarbage.arr, sizeof(int), GARBAGE_CAP, sfl);

    fseek(fl, 0L, SEEK_SET);
    fwrite(&(garbage.top), sizeof(int), 1, fl);
    fwrite(garbage.arr, sizeof(int), GARBAGE_CAP, fl);

    fseek(ind, 0L, SEEK_SET);
    for(int i = 0; i < CLIENT_NUMBER; i++)
    {
        fwrite(&i, sizeof(int), 1, ind);
        fwrite(&(itable[i]), sizeof(int), 1, ind);
    }
    fclose(sfl);
    fclose(fl);
    fclose(ind);
    /*
    insert-m 1 A A
    insert-m 2 B B
    insert-m 3 C C
    insert-m 4 D D
    insert-m 5 E E
    */
    return 0;
}
