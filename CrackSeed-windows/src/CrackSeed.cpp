#include <ctime>
#include <atomic>
#include <iostream>
#include <thread>
#include <vector>
#include <cstring>
#include <CrackSeed.h>
#include <openssl/md5.h>


std::atomic_uint_fast32_t seed=-1;
std::atomic_int cracked = 0;
unsigned char keyc[4] = { 0 };
unsigned char cipherText[10];
time_t start, end;


const unsigned char chars[36] = { 'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z','0','1','2','3','4','5','6','7','8','9' };


 void str_to_hex(const unsigned char *v, int len, unsigned char* hex)
 {
     const unsigned char tbl[0x10] =
     {
       '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
       'a', 'b', 'c', 'd', 'e', 'f',
     };

     for (int i = 0, idx = 0; i < len; i++,idx+=2) {

     hex[idx +1] = tbl[*(v + i) >> 0 & 15];
     hex[idx] = tbl[*(v + i) >> 4 & 15];
     
     }
 }


 void hex_to_str(const unsigned char* hex,int len, unsigned char* v)
 {

     for (int i = 0, idx = 0; i < len; i += 2, idx++) {

         v[idx] = (hex[i] > 0x39 ? hex[i] - 'a'+10 : hex[i] - '0') << 4 | (hex[i+1] > 0x39 ? hex[i + 1] - 'a' +10: hex[i + 1] - '0') << 0;
     }
 }




int is_valid_number(const unsigned char c)
 {
     if ((c >= '0') && (c <= '9')) return 1;

     return 0;
 }

 void crackseed()
 {

     while(!cracked){

     const uint32_t lseed = ++seed;
     if (lseed>0 &&(lseed % 10000000)==0) {
         printf("seed:%u, process: %.2f%%\n", lseed, 100 * ((float)lseed / (float)0xffffffff));
     }


     mt19937_state state;
     mt19937_seed(&state, lseed);

     unsigned char key[19] = { 0 };
     
     for (int i = 0; i < 18; i += 1)
         key[i] = chars[php_mt_rand_range(&state, 1, 0, 35)];
    //printf("seed=%u,key=%s,", lseed, key);
     unsigned char key_md5[16];
     unsigned char key_halfmd5[16] = { 0 };
     unsigned char keya_keyc[68] = { 0 }; //md5+md5+keyc
     unsigned char cryptkey[65] = { 0 };
     
     //md5(key)
     MD5((unsigned char*)&key, 18, (unsigned char*)&key_md5);

     //取md5 hex前16位
     str_to_hex((unsigned char*)&key_md5, 8, (unsigned char*)&key_halfmd5);

     //keya=md5(substr(md5(key),0,16))

     MD5((unsigned char*)&key_halfmd5, 16, (unsigned char*)&key_md5);
     str_to_hex(key_md5, 16, keya_keyc);
     str_to_hex(key_md5, 16, keya_keyc + 32);
     memcpy(keya_keyc + 64, keyc, 4);
     MD5(keya_keyc + 32, 36, (unsigned char*)&key_md5);

     str_to_hex((unsigned char*)&key_md5, 16, keya_keyc + 32);

     memcpy(cryptkey, keya_keyc, 64);
     //printf("cryptokey:%s\n", cryptkey);

     int box[256];
     for (int i = 0; i < 256; i++) {
         box[i] = i;

     }
     for (int i = 0, j = 0; i < 256; i += 1)
     {
         j = (j + box[i] + cryptkey[i & 63]) & 255;
         int tmp = box[i];
         box[i] = box[j];
         box[j] = tmp;

     }

     unsigned char plain_txt[11] = { 0 };
     for (int a = 0, j = 0, i = 0; i < 10; i += 1) {
         a = (a + 1) & 255;
         j = (j + box[a]) & 255;
         int tmp = box[a];
         box[a] = box[j];
         box[j] = tmp;
         plain_txt[i] = cipherText[i] ^ box[(box[a] + box[j]) & 255];

         //前10位非数字则跳出循环
         if (!is_valid_number(plain_txt[i])) break;

         //明文前26位为10位数字+16位md5, 判断字符在此区间则解密成功,这里判断前10位为数字则认为解密成功
         if (i == 9) {
             printf("\nseed: %u\n", lseed);
             printf("cryptkey: %s\n", key);
             printf("plain_txt: %s\n", plain_txt);
             cracked++;
             
             // 计算使用的时间（单位：秒）
             end=time(NULL);

             printf("Time used = %.2f seconds\n", difftime(end,start));
             printf("All done, now exit.\n");
             exit(1);

         }

     }
     }

 }


int main(int argc, unsigned char** argv)
{
    if (argc != 3) {
        printf("Usage: %s [keyc] [cipherText_hex]\n", argv[0]);
        return 0;
    }

    start = time(NULL);
    hex_to_str(argv[2], 20, cipherText);
    memcpy(keyc, argv[1],4);
    uint32_t maxThread_num = std::jthread::hardware_concurrency();
    std::vector<std::jthread> pool;
    for (int n =0; n < std::jthread::hardware_concurrency(); ++n){
        pool.emplace_back(crackseed);
    }

}

