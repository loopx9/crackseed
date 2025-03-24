<?php
function cpEncode($data, $key = '', $expire = 0)
    {
        $string = serialize($data);
        $ckey_length = 4;
        $key = md5($key);

        $keya = md5(substr($key, 0, 16));
        $keyb = md5(substr($key, 16, 16));
        $keyc = substr(md5(microtime()), -$ckey_length);

        $cryptkey = $keya . md5($keya . $keyc);
        $key_length = strlen($cryptkey);

        $string = sprintf('%010d', $expire ? $expire + time() : 0) . substr(md5($string . $keyb), 0, 16) . $string;
        $string_length = strlen($string);
        $result = '';
        $box = range(0, 255);

        $rndkey = array();
        for ($i = 0; $i <= 255; $i++) {
            $rndkey[$i] = ord($cryptkey[$i % $key_length]);
        }

        for ($j = $i = 0; $i < 256; $i++) {
            $j = ($j + $box[$i] + $rndkey[$i]) % 256;
            $tmp = $box[$i];
            $box[$i] = $box[$j];
            $box[$j] = $tmp;
        }

        for ($a = $j = $i = 0; $i < $string_length; $i++) {
            $a = ($a + 1) % 256;
            $j = ($j + $box[$a]) % 256;
            $tmp = $box[$a];
            $box[$a] = $box[$j];
            $box[$j] = $tmp;
            $result .= chr(ord($string[$i]) ^ ($box[($box[$a] + $box[$j]) % 256]));
        }
        
        
        //return substr($result,10,16);
        return ($keyc . " ".substr(bin2hex($result),0,32));
    }
    
    
    
function GetRandStr(){
    //字符组合
    
    $str = 'abcdefghijklmnopqrstuvwxyz0123456789';
    $len = strlen($str)-1;
    $randstr = '';
    for ($i=0;$i<18;$i++) {
        $num=mt_rand(0,$len);
        $randstr .= $str[$num];
    }
    return $randstr;
}


mt_srand(80000000);
$key = GetRandStr(); 


echo cpEncode('hashcat', $key,0)."\n";
echo "key: ".$key.PHP_EOL;

  /*seed=800000000 f45a ea80176958ac0658937a8ba3c0b04f31*/  
    
?>


