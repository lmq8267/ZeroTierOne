#!/bin/sh

PROG=/etc/storage/zerotier-one/zerotier-one
[ ! -f "$PROG" ] && PROG=/tmp/zerotier-one/zerotier-one
PROGCLI=/etc/storage/zerotier-one/zerotier-cli
[ ! -f "$PROGCLI" ] && PROGCLI=/tmp/zerotier-one/zerotier-cli
PROGIDT=/etc/storage/zerotier-one/zerotier-idtool
[ ! -f "$PROGIDT" ] && PROGIDT=/tmp/zerotier-one/zerotier-idtool
config_path="/etc/storage/zerotier-one"
PLANET="/etc/storage/planet"
zeroid="$(nvram get zerotier_id)"
D="/etc/storage/cron/crontabs"
F="$D/`nvram get http_username`"
 
start_instance() {
cfg="$(nvram get zerotier_id)"
echo "zerotier_id=$cfg"
port=""
args=""
moonid="$(nvram get zerotier_moonid)"
planet="$(nvram get zerotier_planet)"
if [ ! -s "/etc/storage/zerotier-one/identity.secret" ] ; then
secret="$(nvram get zerotier_secret)"
else
secret="$(cat /etc/storage/zerotier-one/identity.secret)"
fi
if [ ! -d "$config_path" ]; then
  mkdir -p $config_path
fi
mkdir -p $config_path/networks.d
if [ -n "$port" ]; then
   args="$args -p$port"
fi
if [ -z "$secret" ]; then
   [ ! -n "$cfg" ] && logger -t "【ZeroTier】" "无法启动，即将退出..." ; echo "没有设置zerotier_id，程序退出" && logger -t "【ZeroTier】" "未获取到zerotier id，请确认在自定义设置-脚本-在路由器启动后执行里已填写好zerotier id" && logger -t "【ZeroTier】" "填好后，在系统管理-控制台输入一次nvram set zerotier_id=你的zerotier id" && logger -t "【ZeroTier】" "然后手动启动，在系统管理-控制台输入一次 /etc/storage/zerotier.sh start" && exit 1
   logger -t "【ZeroTier】" "设备密钥为空，正在生成密钥，请稍候..."
   PROG=/etc/storage/zerotier-one/zerotier-one
   [ ! -f "$PROG" ] && PROG=/tmp/zerotier-one/zerotier-one
   PROGCLI=/etc/storage/zerotier-one/zerotier-cli
   [ ! -f "$PROGCLI" ] && PROGCLI=/tmp/zerotier-one/zerotier-cli
   PROGIDT=/etc/storage/zerotier-one/zerotier-idtool
   [ ! -f "$PROGIDT" ] && PROGIDT=/tmp/zerotier-one/zerotier-idtool
   echo "设备密钥为空，正在生成密钥，请稍候..."
   sf="$config_path/identity.secret"
   pf="$config_path/identity.public"
   $PROGIDT generate "$sf" "$pf"  >/dev/null
   [ $? -ne 0 ] && return 1
   secret="$(cat $sf)"
   nvram set zerotier_secret="$secret"
   nvram commit
else
   logger -t "【ZeroTier】" "找到密钥文件，正在启动，请稍候..."
   echo "找到密钥文件，正在启动，请稍候..."
   echo "$secret" >$config_path/identity.secret
   $PROGIDT getpublic $config_path/identity.secret >$config_path/identity.public
fi

if [ -n "$planet"]; then
		logger -t "【ZeroTier】" "找到planet,正在写入..."
		echo "$planet" >$config_path/planet.tmp
		base64 -d $config_path/planet.tmp >$config_path/planet
fi
if [ -f "$PLANET" ]; then
		if [ ! -s "$PLANET" ]; then
			echo "自定义planet文件为空"
		else
			logger -t "【ZeroTier】" "找到自定义planet文件,开始创建..."
			planet="$(base64 $PLANET)"
			cp -f $PLANET $config_path/planet
			rm -f $PLANET
			nvram set zerotier_planet="$planet"
			nvram commit
		fi
fi
$PROG $args $config_path >/dev/null 2>&1 &
while [ ! -f $config_path/zerotier-one.port ]; do
		sleep 1
done
if [ -n "$cfg" ]; then
  $PROGCLI join $cfg
  #logger -t "【ZeroTier】" "join zerotier_id $zeroid ok!"
  rules
fi
if [ -n "$moonid" ]; then
   $PROGCLI -D$config_path orbit $moonid $moonid
   logger -t "【ZeroTier】" "orbit moonid $moonid ok!"
fi
zeromoonip="$(nvram get zeromoonwan)"
moonip="$(nvram get zerotiermoon_ip)"
if [ "$zeromoonip" = "1" ] || [ -n "$moonip" ]; then
   logger -t "【ZeroTier】" "creat moon start!"
   creat_moon
   else
   remove_moon
fi
zerotier_keep
}

rules() {
	while [ "$(ifconfig | grep zt | awk '{print $1}')" = "" ]; do
		sleep 1
	done
	zt0=$(ifconfig | grep zt | awk '{print $1}')
	logger -t "【ZeroTier】" "已创建虚拟网卡 $zt0 "	
        echo "已创建虚拟网卡 $zt0 "
	ip44=$(ifconfig $zt0  | grep "inet addr:" | awk '{print $2}' | awk -F '/' '{print $1}'| tr -d 'addr:' | tr -d ' ')
        ip66=$(ifconfig $zt0  | grep "inet6 addr:" | awk '{print $3}' | awk '{print $1,$2}'| tr -d 'addr' | tr -d ' ')
        [ -n "$ip66" ] && logger -t "【ZeroTier】" ""$zt0"_ipv6:$ip66"
        [ -n "$ip44" ] && logger -t "【ZeroTier】" ""$zt0"_ipv4:$ip44"
        [ -z "$ip44" ] && logger -t "【ZeroTier】" "未获取到zerotier ip请前往官网检查是否勾选此路由加入网络并分配IP"
	count=0
        while [ $count -lt 5 ]
        do
       ztstatus=$(zerotier-cli info | awk '{print $5}')
       if [ "$ztstatus" = "OFFLINE" ]; then
        sleep 3
        elif [ "$ztstatus" = "ONLINE" ]; then
        ztid=$(zerotier-cli info | awk '{print $3}')
        logger -t "【ZeroTier】" "若是官网没有此设备，请手动绑定此设备ID  $ztid "
	echo "若是官网没有此设备，请手动绑定此设备Node Id  $ztid "
        break
        fi
        count=$(expr $count + 1)
        done
	if [ "$(zerotier-cli info | awk '{print $5}')" = "OFFLINE" ] ; then
          echo "你的网络无法连接到zerotier服务器，请检查网络，程序退出"
	  logger -t "【ZeroTier】" "你的网络无法连接到zerotier服务器，请检查网络，程序退出"
          exit 1
        fi
	del_rules
	iptables -I INPUT -i $zt0 -j ACCEPT
	iptables -I FORWARD -i $zt0 -o $zt0 -j ACCEPT
	iptables -I FORWARD -i $zt0 -j ACCEPT
	iptables -t nat -I POSTROUTING -o $zt0 -j MASQUERADE
	while [ "$(ip route | grep "dev $zt0  proto kernel" | awk '{print $1}')" = "" ]; do
	sleep 1
	done
	ip_segment="$(ip route | grep "dev $zt0  proto kernel" | awk '{print $1}')"
	iptables -t nat -A POSTROUTING -s $ip_segment -j MASQUERADE
	logger -t "【ZeroTier】" "启用ZeroTier NAT"
        logger -t "【ZeroTier】" "ZeroTier官网：https://my.zerotier.com/network"
	####访问上级路由其他设备添加路由规则命令##
	#ip route add $zero_ip via $zero_route dev $zt0
	#其中$zero_ip改为zerotier官网分配的ip   $zero_route改为你想要访问的上级路由网段如 192.168.30.0/24     $zt0改为你的zerotier网卡名 如ztoj56Rop2
	#删除命令ip route del $zero_ip via $zero_route dev $zt0
        
}

del_rules() {
	zt0=$(ifconfig | grep zt | awk '{print $1}')
	ip_segment=`ip route | grep "dev $zt0  proto" | awk '{print $1}'`
	iptables -D FORWARD -i $zt0 -j ACCEPT 2>/dev/null
	iptables -D FORWARD -o $zt0 -j ACCEPT 2>/dev/null
	iptables -D FORWARD -i $zt0 -o $zt0 -j ACCEPT 2>/dev/null
	iptables -D INPUT -i $zt0 -j ACCEPT 2>/dev/null
	iptables -t nat -D POSTROUTING -o $zt0 -j MASQUERADE 2>/dev/null
	iptables -t nat -D POSTROUTING -s $ip_segment -j MASQUERADE 2>/dev/null
}

#创建moon节点,zerotier不再支持动态域名
creat_moon(){
moonip="$(nvram get zerotiermoon_ip)"
#检查是否合法ip
regex="\b(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[1-9])\.(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])\.(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])\.(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[1-9])\b"
ckStep2=`echo $moonip | egrep $regex | wc -l`
logger -t "【ZeroTier】" "搭建ZeroTier的Moon中转服务器，生成moon配置文件"
zeromoonip="$(nvram get zeromoonwan)"
if [ "$zeromoonip" = "1" ]; then
   #自动获取wanip
   ip_addr=`ifconfig -a ppp0 | grep 'inet addr:' | cut -d: -f2 | awk '{ print $1}'`
   else
   ip_addr=$moonip
fi
logger -t "【ZeroTier】" "ZeroTier Moon服务器 IP $ip_addr"
if [ -e $config_path/identity.public ]; then
   $PROGIDT initmoon $config_path/identity.public > $config_path/moon.json
   if `sed -i "s/\[\]/\[ \"$ip_addr\/9993\" \]/" $config_path/moon.json >/dev/null 2>/dev/null`; then
       logger -t "【ZeroTier】" "生成moon配置文件成功"
       else
       logger -t "【ZeroTier】" "生成moon配置文件失败"
    fi
   logger -t "【ZeroTier】" "生成签名文件"
   cd $config_path
   pwd
   $PROGIDT genmoon $config_path/moon.json
   [ $? -ne 0 ] && return 1
   logger -t "【ZeroTier】" "创建moons.d文件夹，并把签名文件移动到文件夹内"
   if [ ! -d "$config_path/moons.d" ]; then
      mkdir -p $config_path/moons.d
   fi
   #服务器加入moon server
   mv $config_path/*.moon $config_path/moons.d/ >/dev/null 2>&1
   logger -t "【ZeroTier】" "moon节点创建完成"
   zmoonid=`cat moon.json | awk -F "[id]" '/"id"/{print$0}'` >/dev/null 2>&1
   zmoonid=`echo $zmoonid | awk -F "[:]" '/"id"/{print$2}'` >/dev/null 2>&1
   zmoonid=`echo $zmoonid | tr -d '"|,'`
   nvram set zerotiermoon_id="$zmoonid"
   logger -t "【ZeroTier】" "已生成Moon服务器的ID: $zmoonid"
   else
   logger -t "【ZeroTier】" "identity.public不存在"
 fi  
}
      
remove_moon(){
zmoonid="$(nvram get zerotiermoon_id)"
if [ ! -n "$zmoonid"]; then
  rm -f $config_path/moons.d/000000$zmoonid.moon
  rm -f $config_path/moon.json
  nvram set zerotiermoon_id=""
fi
} 

zero_dl(){
   sleep 2
   zerotier_start
}

zerotier_up(){
    logger -t "【ZeroTier】" "网络中断，重新启动"
    zerotier_start
   zerotier_start
}

zerotier_keep  () {
[ ! -z "`pidof zerotier-one`" ] && logger -t "【ZeroTier】" "启动成功" ; echo "启动成功"
logger -t "【ZeroTier】" "守护进程启动"
sed -Ei '/ZeroTier守护进程|^$/d' "$F"
cat >> "$F" <<-OSC
*/1 * * * * test -z "\`pidof zerotier-one\`"  && /etc/storage/zerotier.sh restart #ZeroTier守护进程
OSC
zero_ping &
}

zero_ping() {
while [ "$(ifconfig | grep zt | awk '{print $1}')" = "" ]; do
		sleep 1
done
zt0=$(ifconfig | grep zt | awk '{print $1}')
while [ "$(ip route | grep "dev $zt0  proto static" | awk '{print $1}' | awk -F '/' '{print $1}')" = "" ]; do
sleep 1
done
ip00=$(ip route | grep "dev "$zt0"  proto static" | awk '{print $1}' | awk -F '/' '{print $1}')
[ -n "$ip00" ] && logger -t "【ZeroTier】" "zerotier虚拟局域网内设备：$ip00 "
ip11=$(ip route | grep "dev "$zt0"  proto static" | awk '{print $1}' | awk -F '/' '{print $1}'| awk 'NR==1 {print $1}'|cut -d. -f1,2,3)
ip22=$(ip route | grep "dev "$zt0"  proto static" | awk '{print $1}' | awk -F '/' '{print $1}'| awk 'NR==2 {print $1}'|cut -d. -f1,2,3)
ip33=$(ip route | grep "dev "$zt0"  proto static" | awk '{print $1}' | awk -F '/' '{print $1}'| awk 'NR==3 {print $1}'|cut -d. -f1,2,3)
ip44=$(ip route | grep "dev "$zt0"  proto static" | awk '{print $1}' | awk -F '/' '{print $1}'| awk 'NR==4 {print $1}'|cut -d. -f1,2,3)
ip55=$(ip route | grep "dev "$zt0"  proto static" | awk '{print $1}' | awk -F '/' '{print $1}'| awk 'NR==5 {print $1}'|cut -d. -f1,2,3)
sleep 20
[ -n "$ip11" ] && ping_zero1=$(ping -4 $ip11.1 -c 2 -w 4 -q)
[ -n "$ip22" ] && ping_zero2=$(ping -4 $ip22.1 -c 2 -w 4 -q)
[ -n "$ip33" ] && ping_zero3=$(ping -4 $ip33.1 -c 2 -w 4 -q)
[ -n "$ip44" ] && ping_zero4=$(ping -4 $ip44.1 -c 2 -w 4 -q)
[ -n "$ip55" ] && ping_zero5=$(ping -4 $ip55.1 -c 2 -w 4 -q)
[ -n "$ip11" ] && ping_time1=`echo $ping_zero1 | awk -F '/' '{print $4}'`
[ -n "$ip22" ] && ping_time2=`echo $ping_zero2 | awk -F '/' '{print $4}'`
[ -n "$ip33" ] && ping_time3=`echo $ping_zero3 | awk -F '/' '{print $4}'`
[ -n "$ip44" ] && ping_time4=`echo $ping_zero4 | awk -F '/' '{print $4}'`
[ -n "$ip55" ] && ping_time5=`echo $ping_zero5 | awk -F '/' '{print $4}'`
[ -n "$ip11" ] && ping_loss1=`echo $ping_zero1 | awk -F ', ' '{print $3}' | awk '{print $1}'`
[ -n "$ip22" ] && ping_loss2=`echo $ping_zero2 | awk -F ', ' '{print $3}' | awk '{print $1}'`
[ -n "$ip33" ] && ping_loss3=`echo $ping_zero3 | awk -F ', ' '{print $3}' | awk '{print $1}'`
[ -n "$ip44" ] && ping_loss4=`echo $ping_zero4 | awk -F ', ' '{print $3}' | awk '{print $1}'`
[ -n "$ip55" ] && ping_loss5=`echo $ping_zero5 | awk -F ', ' '{print $3}' | awk '{print $1}'`
[ ! -z "$ping_time1" ] && logger -t "【ZeroTier】" "已连通"$ip11".1，延迟:$ping_time1 ms 丢包率：$ping_loss1 "
[ ! -z "$ping_time2" ] && logger -t "【ZeroTier】" "已连通"$ip22".1，延迟:$ping_time2 ms 丢包率：$ping_loss2 "
[ ! -z "$ping_time3" ] && logger -t "【ZeroTier】" "已连通"$ip33".1，延迟:$ping_time3 ms 丢包率：$ping_loss3 "
[ ! -z "$ping_time4" ] && logger -t "【ZeroTier】" "已连通"$ip44".1，延迟:$ping_time4 ms 丢包率：$ping_loss4 "
[ ! -z "$ping_time5" ] && logger -t "【ZeroTier】" "已连通"$ip55".1，延迟:$ping_time5 ms 丢包率：$ping_loss5 "

}

zerotier_close () {
del_rules
sed -Ei '/ZeroTier守护进程|^$/d' "$F"
killall zerotier-one
killall -9 zerotier-one
[ -d /tmp/zerotier-one ] && rm -rf /tmp/zerotier-one
[ -z "`pidof zerotier-one`" ] && logger -t "【ZeroTier】" "进程已关闭"
}

zerotier_start()  {
killall -9 zerotier-one
SVC_PATH="/etc/storage/zerotier-one/zerotier-one"
[ ! -f "$SVC_PATH" ] && SVC_PATH="/tmp/zerotier-one/zerotier-one" && [ ! -d /tmp/zerotier-one ] && mkdir -p /tmp/zerotier-one
zerosize=$(df -m | grep "% /etc" | awk 'NR==1' | awk -F' ' '{print $4}'| tr -d 'M' | tr -d '')
tag="$( curl -k --connect-timeout 20 -s https://api.github.com/repos/lmq8267/ZeroTierOne/releases/latest  | grep 'tag_name' | cut -d\" -f4 )"
[ -z "$tag" ] && tag="$( curl -k -L --connect-timeout 20 --silent https://api.github.com/repos/lmq8267/ZeroTierOne/releases/latest | grep 'tag_name' | cut -d\" -f4 )"
[ -z "$tag" ] && tag="$(curl -k --silent "https://api.github.com/repos/lmq8267/ZeroTierOne/releases/latest" | grep '"tag_name":' | sed -E 's/.*"([^"]+)".*/\1/')"
if [ ! -s "$SVC_PATH" ] ; then
   rm -rf /etc/storage/zerotier-one/MD5.txt
   logger -t "【ZeroTier】" "找不到zerotiero-one,开始下载"
   if [ "$zerosize" -lt 2 ];then
      logger -t "【ZeroTier】" "您的设备/etc/storage空间剩余"$zerosize"M，不足2M，将下载安装包到内存安装"
      if [ ! -z "$tag" ] ; then
      logger -t "【ZeroTier】" "获取到最新版本$tag ,开始下载"
      curl -L -k -S -o "/tmp/zerotier-one/zerotier-one" --connect-timeout 10 --retry 3 "https://hub.gitmirror.com/https://github.com/lmq8267/ZeroTierOne/blob/dev/install/$tag/zerotier-one" || curl -L -k -S -o "/tmp/zerotier-one/zerotier-one" --connect-timeout 10 --retry 3 "https://fastly.jsdelivr.net/gh/lmq8267/ZeroTierOne@master/install/$tag/zerotier-one"
      curl -L -k -S -o "/etc/storage/zerotier-one/MD5.txt" --connect-timeout 10 --retry 3 "https://hub.gitmirror.com/https://github.com/lmq8267/ZeroTierOne/blob/dev/install/$tag/MD5.txt" || curl -L -k -S -o "/etc/storage/zerotier-one/MD5.txt" --connect-timeout 10 --retry 3 "https://fastly.jsdelivr.net/gh/lmq8267/ZeroTierOne@master/install/$tag/MD5.txt"
      else
      logger -t "【ZeroTier】" "未获取到最新版本号 ,开始下载1.10.6版本"
      curl -L -k -S -o "/tmp/zerotier-one/zerotier-one" --connect-timeout 10 --retry 3 "https://hub.gitmirror.com/https://github.com/lmq8267/ZeroTierOne/blob/dev/install/1.10.6/zerotier-one" || curl -L -k -S -o "/tmp/zerotier-one/zerotier-one" --connect-timeout 10 --retry 3 "https://fastly.jsdelivr.net/gh/lmq8267/ZeroTierOne@master/install/1.10.6/zerotier-one"
      curl -L -k -S -o "/etc/storage/zerotier-one/MD5.txt" --connect-timeout 10 --retry 3 "https://hub.gitmirror.com/https://github.com/lmq8267/ZeroTierOne/blob/dev/install/1.10.6/MD5.txt" || curl -L -k -S -o "/etc/storage/zerotier-one/MD5.txt" --connect-timeout 10 --retry 3 "https://fastly.jsdelivr.net/gh/lmq8267/ZeroTierOne@master/install/1.10.6/MD5.txt"
      fi
      else
      SVC_PATH="/etc/storage/zerotier-one/zerotier-one"
      if [ ! -z "$tag" ] ; then
      logger -t "【ZeroTier】" "获取到最新版本$tag ,开始下载"
      curl -L -k -S -o "/etc/storage/zerotier-one/zerotier-one" --connect-timeout 10 --retry 3 "https://hub.gitmirror.com/https://github.com/lmq8267/ZeroTierOne/blob/dev/install/$tag/zerotier-one" || curl -L -k -S -o "/etc/storage/zerotier-one/zerotier-one" --connect-timeout 10 --retry 3 "https://fastly.jsdelivr.net/gh/lmq8267/ZeroTierOne@master/install/$tag/zerotier-one"
      curl -L -k -S -o "/etc/storage/zerotier-one/MD5.txt" --connect-timeout 10 --retry 3 "https://hub.gitmirror.com/https://github.com/lmq8267/ZeroTierOne/blob/dev/install/$tag/MD5.txt" || curl -L -k -S -o "/etc/storage/zerotier-one/MD5.txt" --connect-timeout 10 --retry 3 "https://fastly.jsdelivr.net/gh/lmq8267/ZeroTierOne@master/install/$tag/MD5.txt"
      else
      logger -t "【ZeroTier】" "未获取到最新版本号 ,开始下载1.10.6版本"
      curl -L -k -S -o "/etc/storage/zerotier-one/zerotier-one" --connect-timeout 10 --retry 3 "https://hub.gitmirror.com/https://github.com/lmq8267/ZeroTierOne/blob/dev/install/1.10.6/zerotier-one" || curl -L -k -S -o "/etc/storage/zerotier-one/zerotier-one" --connect-timeout 10 --retry 3 "https://fastly.jsdelivr.net/gh/lmq8267/ZeroTierOne@master/install/1.10.6/zerotier-one"
      curl -L -k -S -o "/etc/storage/zerotier-one/MD5.txt" --connect-timeout 10 --retry 3 "https://hub.gitmirror.com/https://github.com/lmq8267/ZeroTierOne/blob/dev/install/1.10.6/MD5.txt" || curl -L -k -S -o "/etc/storage/zerotier-one/MD5.txt" --connect-timeout 10 --retry 3 "https://fastly.jsdelivr.net/gh/lmq8267/ZeroTierOne@master/install/1.10.6/MD5.txt"
      fi
   fi
fi
if [ ! -s "/etc/storage/zerotier-one/zerotier-one" ] && [ ! -s "/tmp/zerotier-one/zerotier-one" ]; then
   logger -t "【ZeroTier】" "下载失败，重新下载"
   rm -rf /etc/storage/zerotier-one/zerotier-one /tmp/zerotier-one/zerotier-one
   zero_dl
fi
if [ -s "/etc/storage/zerotier-one/MD5.txt" ] ; then
   zeroMD5="$(cat /etc/storage/zerotier-one/MD5.txt)"
   [ -s "/etc/storage/zerotier-one/zerotier-one" ] && eval $(md5sum "/etc/storage/zerotier-one/zerotier-one" | awk '{print "MD5_down="$1;}') && echo "$MD5_down"
   [ -s "/tmp/zerotier-one/zerotier-one" ] && eval $(md5sum "/tmp/zerotier-one/zerotier-one" | awk '{print "MD5_down="$1;}') && echo "$MD5_down"
   if [ "$zeroMD5"x = "$MD5_down"x ] ; then
       logger -t "【ZeroTier】" "程序下载完成，MD5匹配，开始安装..."
   else
       logger -t "【ZeroTier】" "程序下载完成，MD5不匹配，删除重新下载..."
       rm -rf /etc/storage/zerotier-one/zerotier-one /tmp/zerotier-one/zerotier-one
       zero_dl
   fi
else
   logger -t "【ZeroTier】" "下载失败，重新下载"
   rm -rf /etc/storage/zerotier-one/zerotier-one /tmp/zerotier-one/zerotier-one
   zero_dl
fi
if [ ! -s "$SVC_PATH" ] ; then
   logger -t "【ZeroTier】" "下载失败，10秒后尝试重新下载"
   sleep 10
   zero_dl
else
   if [ -f /tmp/zerotier-one/zerotier-one ] ; then
   chmod 777 /tmp/zerotier-one/zerotier-one 
   rm -rf /tmp/zerotier-one/zerotier-cli 
   rm -rf /tmp/zerotier-one/zerotier-idtool
   ln -sf /tmp/zerotier-one/zerotier-one /tmp/zerotier-one/zerotier-cli 
   ln -sf /tmp/zerotier-one/zerotier-one /tmp/zerotier-one/zerotier-idtool
   fi
   if [ -f /etc/storage/zerotier-one/zerotier-one ] ; then
   chmod 777 /etc/storage/zerotier-one/zerotier-one
   rm -rf /etc/storage/zerotier-one/zerotier-cli 
   rm -rf /etc/storage/zerotier-one/zerotier-idtool 
   ln -sf /etc/storage/zerotier-one/zerotier-one /etc/storage/zerotier-one/zerotier-cli 
   ln -sf /etc/storage/zerotier-one/zerotier-one /etc/storage/zerotier-one/zerotier-idtool
   fi
fi
if [ -f "$SVC_PATH" ] ; then
       zerotier_v=$($SVC_PATH -version | sed -n '1p')
       echo "$tag"
       echo "$zerotier_v"
      [ ! -z "$zerotier_v" ] && logger -t "【ZeroTier】" " $SVC_PATH 安装成功，版本号:v$zerotier_v "
       if [ ! -z "$tag" ] && [ ! -z "$zerotier_v" ] ; then
          if [ "$tag"x != "$zerotier_v"x ] ; then
             cat /etc/storage/started_script.sh|grep zerotier_upgrade=y >/dev/null
	      if [ $? -eq 0 ] ; then
               logger -t "【ZeroTier】" "检测到最新版本zerotier_v$tag,当前安装版本zerotier_v$zerotier_v,已开启自动更新，开始更新"
	        rm -rf /etc/storage/zerotier-one/zerotier-one
	        rm -rf /tmp/zerotier-one
                zero_dl
                else
               logger -t "【ZeroTier】" "检测到最新版本zerotier_v$tag,当前安装版本zerotier_v$zerotier_v,未开启自动更新,跳过更新"
	      fi
           fi
       fi
else
logger -t "【ZeroTier】" "下载失败，重新下载"
   rm -rf /etc/storage/zerotier-one/zerotier-one /tmp/zerotier-one/zerotier-one
   zero_dl
fi
start_instance 'zerotier'

}

zerotier_restart () {
if [ -z "`pidof zerotier-one`" ] ; then
    logger -t "【ZeroTier】" "重新启动"
    zerotier_start
fi

}

case $1 in
start)
	zerotier_start
	;;
check)
	zerotier_start
	;;
stop)
	zerotier_close
	;;
keep)
	#zerotier_keep
	zerotier_keep
	;;
up)
	zerotier_up
	;;
restart)
	zerotier_close
	zerotier_start
	;;

*)
	zerotier_start
	;;
esac

