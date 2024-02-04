#!/bin/sh
if [ -f "/etc_ro/script.tgz" ] && [ -f "/etc/storage/www_sh/menu_title.sh" ] ; then
logger -t "【ZeroTier】" "开始从GitHub下载脚本，请稍候..."
echo "开始从GitHub下载脚本，请稍候..."
if [ ! -d "/etc/storage/zerotier-one" ] ; then
  mkdir -p /etc/storage/zerotier-one
fi
if [ -f "/etc/storage/zerotier.sh" ] ; then
mkdir -p /etc/storage/zerotierbackup
echo "检测到已有/etc/storage/zerotier.sh，脚本冲突,已移动到/etc/storage/zerotierbackup/zerotier.sh"
mv -f /etc/storage/zerotier.sh /etc/storage/zerotierbackup/zerotier.sh
[ -f "/etc/storage/zerotierbackup/zerotier.sh" ] && logger -t "【ZeroTier】" "检测到已有/etc/storage//zerotier.sh，脚本冲突,已移动到/etc/storage/zerotierbackup/zerotier.sh"
fi 
if [ ! -e "/etc/storage/zerotier.sh" ] || [ ! -s "/etc/storage/zerotier.sh" ] ; then
 wgetcurl.sh "/etc/storage/zerotier.sh" "https://hub.gitmirror.com/https://github.com/lmq8267/ZeroTierOne/raw/dev/install/hiboyzerotier.sh" "https://fastly.jsdelivr.net/gh/lmq8267/ZeroTierOne@master/install/hiboyzerotier.sh"
fi
if [ ! -s "/etc/storage/zerotier.sh" ] ; then
logger -t "【ZeroTier】" "下载失败，请稍后再试，或使用手动上传，退出下载"
echo "下载失败，请稍后再试，或使用手动上传，退出下载"
exit 1 
fi
if [ -s "/etc/storage/zerotier.sh" ] ; then
chmod 777 /etc/storage/zerotier.sh
echo "下载完成，开始写入启动参数到-自定义设置-脚本-在路由器启动后执行里"
logger -t "【ZeroTier】" "下载完成，开始写入启动参数到-自定义设置-脚本-在路由器启动后执行里"
cat /etc/storage/started_script.sh | grep -o 'zerotier_moonid' &>/dev/null
if [ $? -ne 0 ]; then
cat >> "/etc/storage/started_script.sh" <<-OSC

#################zerotier启动参数#################################
#填写你在zerotier官网创建的网络ID，填写格式如:nvram set zerotier_id=6cccb567v880adf8
nvram set zerotier_id=

#填写Moon服务器生成的ID，没有则不填，填写格式如:=a56c826623
nvram set zerotier_moonid=

#ZeroTier Moon服务器 IP，必须公网IP,填写格式如=175.13.156.223
nvram set zerotiermoon_ip=

#下方填=1将使用Wan口获得的IP作为服务器 IP（请确认Wan口为公网IP）
nvram set zeromoonwan=

#zerotier自动更新版本,留空不启用，启用填=y
zerotier_upgrade=

#启用开机自启              
/etc/storage/zerotier.sh start &
#################################################################

OSC

logger -t "【ZeroTier】" "写入完成，请1.在自定义设置-脚本-在路由器启动后执行里填入zerotier_id并应用保存设置"
echo  "写入完成，请1.在自定义设置-脚本-在路由器启动后执行里填入zerotier_id并应用保存设置"
logger -t "【ZeroTier】" "2.在系统管理-控制台输入nvram set zerotier_id=你的zerotier id 命令一次"
echo  "2.在此页面输入nvram set zerotier_id=你的zerotier id 命令一次"
logger -t "【ZeroTier】" "3.打开ttyd或者ssh输入zerotier start 命令手动启动 或者直接重启路由" 
echo "3.在此页面输入zerotier start 命令手动启动 或者直接重启路由"
else
echo "自定义设置-脚本-在路由启动后执行里已有相关启动参数无法写入"
logger -t "【ZeroTier】" "自定义设置-脚本-在路由启动后执行里已有相关启动参数无法写入"
logger -t "【ZeroTier】" "请打开恩山论坛帖子参照教程在自定义设置-脚本-在路由器启动后执行里填入启动参数"
echo  "请打开恩山论坛帖子参照教程在自定义设置-脚本-在路由器启动后执行里填入启动参数"
fi
plb=$(find / -name "identity.public")
plb1=$(find / -name "authtoken.secret")
plb2=$(find / -name "identity.secret")
[ ! -d /etc/storage/zerotier-one ] && mkdir -p /etc/storage/zerotier-one
[ -f $plb ] && [ ! -s /etc/storage/zerotier-one/identity.public ] && cp -rf $plb /etc/storage/zerotier-one/identity.public
[ -f $plb1 ] && [ ! -s /etc/storage/zerotier-one/authtoken.secret ] && cp -rf $plb1 /etc/storage/zerotier-one/authtoken.secret
[ -f $plb2 ] && [ ! -s /etc/storage/zerotier-one/identity.secret ] && cp -rf $plb2 /etc/storage/zerotier-one/identity.secret
if [ -f "/etc/storage/zerotier-one/identity.public" ] && [ -f "/etc/storage/zerotier-one/identity.public" ] && [ -f "/etc/storage/zerotier-one/identity.public" ] ; then
chmod 600 /etc/storage/zerotier-one/identity.public
chmod 600 /etc/storage/zerotier-one/authtoken.secret
chmod 600 /etc/storage/zerotier-one/identity.secret
echo  "找到已使用的zerotier密钥，开始启动zerotier"
echo  "请不要忘记在自定义设置-脚本-在路由器启动后执行里填入zerotier_id并应用保存设置"
/etc/storage/zerotier.sh start &
exit 0 
fi
fi
else
logger -t "【ZeroTier】" "检测当前padavan不是hiboy版的，开始下载其他版padavan脚本"
echo "检测当前padavan不是hiboy版的，开始下载其他版padavan脚本"
if [ -f "/etc/storage/zerotier.sh" ] ; then
mkdir -p /etc/storage/zerotierbackup
echo "检测到已有/etc/storage/zerotier.sh，脚本冲突,已移动到/etc/storage/zerotierbackup/zerotier.sh"
mv -f /etc/storage/zerotier.sh /etc/storage/zerotierbackup/zerotier.sh
[ -f "/etc/storage/zerotierbackup/zerotier.sh" ] && logger -t "【ZeroTier】" "检测到已有/etc/storage/zerotier.sh，脚本冲突,已移动到/etc/storage/zerotierbackup/zerotier.sh"
fi
if [ ! -d "/etc/storage/zerotier-one" ] ; then
  mkdir -p /etc/storage/zerotier-one
fi
logger -t "【ZeroTier】" "开始从GitHub下载脚本，请稍候..."
echo "开始从GitHub下载脚本，请稍候..."
if [ ! -f "/etc/storage/zerotier.sh" ] ; then
curl -L -k -S -o "/etc/storage/zerotier.sh" --connect-timeout 10 --retry 3 "https://fastly.jsdelivr.net/gh/lmq8267/ZeroTierOne@master/install/zerotier.sh" || curl -L -k -S -o "/etc/storage/zerotier.sh" --connect-timeout 10 --retry 3 "https://hub.gitmirror.com/https://github.com/lmq8267/ZeroTierOne/raw/dev/install/zerotier.sh"
fi
if [ ! -s "/etc/storage/zerotier.sh" ] ; then
logger -t "【ZeroTier】" "下载失败，请稍后再试，或使用手动上传，退出下载"
echo "下载失败，请稍后再试，或使用手动上传，退出下载"
exit 1 
fi
if [ -s "/etc/storage/zerotier.sh" ] ; then
chmod 777 /etc/storage/zerotier.sh
echo "下载完成，开始写入启动参数到-参数设置-脚本-在路由器启动后执行里"
logger -t "【ZeroTier】" "下载完成，开始写入启动参数到-参数设置-脚本-在路由器启动后执行里"
cat /etc/storage/started_script.sh | grep -o 'zerotier_moonid' &>/dev/null
if [ $? -ne 0 ]; then
cat >> "/etc/storage/started_script.sh" <<-OSC

#################zerotier启动参数#################################
#填写你在zerotier官网创建的网络ID，填写格式如:nvram set zerotier_id=6cccb567v880adf8
nvram set zerotier_id=

#填写Moon服务器生成的ID，没有则不填，填写格式如:=a56c826623
nvram set zerotier_moonid=

#ZeroTier Moon服务器 IP，必须公网IP,填写格式如=175.13.156.223
nvram set zerotiermoon_ip=

#下方填=1将使用Wan口获得的IP作为服务器 IP（请确认Wan口为公网IP）
nvram set zeromoonwan=

#zerotier自动更新版本,留空不启用，启用填=y
zerotier_upgrade=

#无需设置开机自启              
#使用此命令启动后 开机后会自启 ：/etc/storage/zerotier.sh start &
#################################################################

OSC

logger -t "【ZeroTier】" "写入完成，请1.在参数设置-脚本-在路由器启动后执行里填入zerotier_id并应用保存设置"
echo  "写入完成，请1.在参数设置-脚本-在路由器启动后执行里填入zerotier_id并应用保存设置"
logger -t "【ZeroTier】" "2.在系统管理-控制台输入nvram set zerotier_id=你的zerotier id 命令一次"
echo  "2.在此页面输入nvram set zerotier_id=你的zerotier id 命令一次"
logger -t "【ZeroTier】" "3.打开ttyd或者ssh输入/etc/storage/zerotier.sh start 命令手动启动 或者直接重启路由" 
echo "3.在此页面输入/etc/storage/zerotier.sh start 命令手动启动 或者直接重启路由"
else
echo "参数设置-脚本-在路由启动后执行里已有相关启动参数无法写入"
logger -t "【ZeroTier】" "参数设置-脚本-在路由启动后执行里已有相关启动参数无法写入"
logger -t "【ZeroTier】" "请打开恩山论坛帖子参照教程在参数设置-脚本-在路由器启动后执行里填入启动参数"
echo  "请打开恩山论坛帖子参照教程在参数设置-脚本-在路由器启动后执行里填入启动参数"
fi
plb=$(find / -name "identity.public")
plb1=$(find / -name "authtoken.secret")
plb2=$(find / -name "identity.secret")
[ ! -d /etc/storage/zerotier-one ] && mkdir -p /etc/storage/zerotier-one
[ -f $plb ] && [ ! -s /etc/storage/zerotier-one/identity.public ] && cp -f $plb /etc/storage/zerotier-one/identity.public
[ -f $plb1 ] && [ ! -s /etc/storage/zerotier-one/authtoken.secret ] && cp -f $plb1 /etc/storage/zerotier-one/authtoken.secret
[ -f $plb2 ] && [ ! -s /etc/storage/zerotier-one/identity.secret ] && cp -f $plb2 /etc/storage/zerotier-one/identity.secret
if [ -f "/etc/storage/zerotier-one/identity.public" ] && [ -f "/etc/storage/zerotier-one/identity.public" ] && [ -f "/etc/storage/zerotier-one/identity.public" ] ; then
chmod 600 /etc/storage/zerotier-one/identity.public
chmod 600 /etc/storage/zerotier-one/authtoken.secret
chmod 600 /etc/storage/zerotier-one/identity.secret
echo  "找到已使用的zerotier密钥，开始启动zerotier"
echo  "请不要忘记在参数设置-脚本-在路由器启动后执行里填入zerotier_id并应用保存设置"
[ -s /usr/bin/zerotier.sh ] && nvram set zerotier_enable=0 && zerotier.sh stop
/etc/storage/zerotier.sh start &
exit 0 
fi
fi
fi

