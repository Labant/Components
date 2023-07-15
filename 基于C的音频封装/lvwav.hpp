/****
 *@Description:基于c的封装WAV音频。可以设置通道、采样数据、采样位数。
 *@Anthor:lvs
 *@Date:2023-07-15
 *
 *@functions:
 *1、创建文件和打开文件:CreatWaveFile:bool-->是否创建能成功
 *@parameters:
 *  (1) filename:文件名。可绝对路径和相对路径，本接口后缀需要为.wav。例如：lvstest.wav
 *  (2) category:打开文件方式。可选r、w、rb、wb、rb+、wb+等，请参考fopen(,)
 *  (3) fs:采样频率
 *  (4) bitsber_sample:采样位数(即采样深度,测试时使用16bit)
 *  (5) channle_number:通道数(测试时4通道)
 *2、添加数据:WaveFileAppend:void
 *@parameters:
 *  (1) &data:int形数据的引用
 *3、关闭文件(生成文件):WriteClose:void
 *@paremeter:
 *  nothing
 */
#ifndef LVWAV_H
#define LVWAV_H
#ifndef WAVFILEHEADER
#define WAVFILEHEADER
struct WAVFILEHEADER
{
    // RIFF 头
    char RiffName[4];
    unsigned long nRiffLength;//Riff头之后的所有数据,即riff头占8B,总头加数据长度-8(就是出去RIFF头(8bits)后的长度)  需要手动设置

    // 数据类型标识符
    char WavName[4];

    // 格式块中的块头
    char FmtName[4];
    unsigned long nFmtLength;

    // 格式块中的块数据
    unsigned short nAudioFormat;
    unsigned short nChannleNumber;
    unsigned long nSampleRate;
    unsigned long nBytesPerSecond;
    unsigned short nBytesPerSample;
    unsigned short nBitsPerSample;

    // 数据块中的块头
    char    DATANAME[4];
    unsigned long   nDataLength;//媒体数据长度需要
}WavFileHeader;
#endif
#include <stdlib.h>
#include <stdio.h>
#include <cstring>
class LVWAV
{

private:
    FILE* pPrivateFile_Pointer;//文件流指针
    long long mCount = 44;//预置结构体长度
    long mDataLen = 0;// nDataLength 总数据流长度
    long mLen = 0;//nRiffLength  = nDataLength + mCount - RIFF(8Bits)
public:
    LVWAV(){};
    ~LVWAV(){if(this->pPrivateFile_Pointer != NULL)this->WriteClose();}//强制写入

//  1、创建文件和打开文件 -->bool:是否创建能成功
    bool CreatWaveFile(char* filename,char* category,int fs, int bitsber_sample, int channle_number){
        this->pPrivateFile_Pointer = fopen(filename,category);
        if(this->pPrivateFile_Pointer == NULL)
            return false;
        if(this->SetWavHeader(this->pPrivateFile_Pointer,fs,bitsber_sample,channle_number) < 44)//预设结构体
            return false;
        return true;
    };
//  2、添加数据 this->mCount需要是字节数 -->void
    void WaveFileAppend(int& data){this->mDataLen += fwrite(&data,1,1,this->pPrivateFile_Pointer);};//第一个是size,第二个是blank
//  3、关闭文件(生成文件)   -->void
    void WriteClose(){
        //根据数据长度修改 nRiffLength nDataLength
        //写入 nRiffLength = 总数据长度-8
        fseek(this->pPrivateFile_Pointer,4,SEEK_SET);
        this->mLen = this->mDataLen + this->mCount - 8;
        fwrite(&this->mLen,1,4,this->pPrivateFile_Pointer);
        //写入 nDataLength
        fseek(this->pPrivateFile_Pointer,40,SEEK_SET);
        fwrite(&this->mDataLen,1,4,this->pPrivateFile_Pointer);
        //关闭WAV文件写入并生成
        fclose(this->pPrivateFile_Pointer);
        free(this->pPrivateFile_Pointer);
        this->SetZeroToMems();//归零
    };
private:
    //辅助函数
    //写入数据头 -->写入长度
    int SetWavHeader(FILE* pfile,int Fs, int bitsber_sample, int channle_number){
        //Wav头
        strcpy(WavFileHeader.RiffName, "RIFF");
        strcpy(WavFileHeader.WavName, "WAVE");
        strcpy(WavFileHeader.FmtName, "fmt ");//加空格是为满足4字节对齐
        strcpy(WavFileHeader.DATANAME, "data");
        // 表示 FMT块 的长度
        WavFileHeader.nFmtLength = 16;//16表示数据块中没有附加信息，18是有附加信息
        // 表示 按照PCM 编码;
        WavFileHeader.nAudioFormat = 1;//大于1时表示有压缩
        // 声道数目;
        WavFileHeader.nChannleNumber = channle_number;//1为单通道（声道），2双通道（声道）
        // 采样频率;
        WavFileHeader.nSampleRate = Fs;//每秒的样本数
        // nBytesPerSample 和 nBytesPerSecond这两个值通过设置的参数计算得到;
        // 数据块对齐单位(每个采样需要的字节数 = 通道数 × 每次采样得到的样本数据位数 / 8 )
        WavFileHeader.nBytesPerSample = channle_number * bitsber_sample / 8;//数据块对齐单位(每个采样需要的字节数)
        // 波形数据传输速率
        // (每秒平均字节数 = 采样频率 × 通道数 × 每次采样得到的样本数据位数 / 8  = 采样频率 × 每个采样需要的字节数 )
        WavFileHeader.nBytesPerSecond = Fs * channle_number * bitsber_sample / 8;//波形数据传输速率（每秒平均字节数）
        // 每次采样得到的样本数据位数;
        WavFileHeader.nBitsPerSample = bitsber_sample;
        //预写入结构头
        return fwrite(&WavFileHeader,1,sizeof(WavFileHeader),pfile);
    }
    //辅助成员归零
    void SetZeroToMems(){this->mDataLen = this->mLen = 0;};
};

#endif // LVWAV_H
