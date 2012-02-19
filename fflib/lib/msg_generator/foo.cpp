
struct student_t
{
    struct book_t
    {
        int16       pages;
        string      contents;
    };
    int8                age;
    float               grade;
    string              name;
    array<string>       friends;
    book_t              mybook;
    dictionary<string,string> note;
};
