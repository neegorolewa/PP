using System.Diagnostics;

internal class Program
{
    static async Task Main(string[] args)
    {
        Console.WriteLine("Ведите путь к файлу: ");
        string filePath = Console.ReadLine();

        if (!File.Exists(filePath)) 
        {
            Console.WriteLine("Файл не найден");
        }

        Console.WriteLine("Введите символы для удаления: ");
        string delSymbol = Console.ReadLine();

        string text = await ReadFile(filePath);
        string newText = DeleteSymbol(delSymbol, text);
        await WriteToFile(newText, filePath);

        Console.WriteLine("Измененный текст сохранен в файле");
    }

    static async Task<string> ReadFile(string filePath)
    {
        using (StreamReader sr = new StreamReader(filePath)) 
        {
            return await sr.ReadToEndAsync();
        }
    }

    static string DeleteSymbol(string delSymbol, string text) 
    {
        string result = "";

        foreach (char c in text)
        {
            if (!delSymbol.Contains(c))
            {
                result += c;
            }
        }

        return result;
    }

    static async Task WriteToFile(string text, string filePath)
    {
        using (StreamWriter sw = new StreamWriter(filePath))
        {
            await sw.WriteAsync(text);
        }
    }
}