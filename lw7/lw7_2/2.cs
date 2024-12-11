using static System.Net.WebRequestMethods;
using System;
using System.Diagnostics;
using System.Net.Http;
using System.Threading.Tasks;
using System.Text.Json;


internal class Program
{
    static async Task Main(string[] args)
    {
        Console.WriteLine("Download images");

        int numImages = 10;

        await DownloadImagesParallelAsync(numImages);

        //await DownloadImagesSequentAsync(numImages);
    }

    static async Task DownloadImagesParallelAsync(int numImages)
    {
        Stopwatch sw = new Stopwatch();
        sw.Start();

        Task[] tasks = new Task[numImages];
        for (int i = 0;  i < numImages; i++)
        {
            tasks[i] = DownloadImageAsync();
        }

        await Task.WhenAll(tasks);

        sw.Stop();
        Console.WriteLine($"Success! Time: {sw.ElapsedMilliseconds}");
    }

    static async Task DownloadImagesSequentAsync(int numImages)
    {
        Stopwatch sw = new Stopwatch();
        sw.Start();

        for (int i = 0; i < numImages; i++)
        {
            await DownloadImageAsync();
        }

        sw.Stop();
        Console.WriteLine($"Success! Time: {sw.ElapsedMilliseconds}");
    }

    static async Task DownloadImageAsync()
    {
        string urlAPI = "https://dog.ceo/api/breeds/image/random";

        try
        {
            using (HttpClient client = new HttpClient())
            {
                HttpResponseMessage response = await client.GetAsync(urlAPI);

                if (response.IsSuccessStatusCode)
                {
                    string json = await response.Content.ReadAsStringAsync();

                    DogApiResponse dogResponse = JsonSerializer.Deserialize<DogApiResponse>(json);

                    Console.WriteLine($"Starting download from url: {dogResponse.message}");

                    if (dogResponse.status == "success") 
                    {
                        Console.WriteLine($"Image from {dogResponse.message} download succesfully!");
                    }
                    else
                    {
                        Console.WriteLine($"Error1: {dogResponse.status}");
                    }
                }
                else
                {
                    Console.WriteLine($"Error2: {response.StatusCode}");
                }
            }
        }
        catch (Exception ex)
        {
            Console.WriteLine(ex.Message );
        }
    }
}

public class DogApiResponse
{
    public string message { get; set; }
    public string status { get; set; }
}