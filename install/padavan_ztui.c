#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <ctype.h>
#include <arpa/inet.h>


const char *html_response = "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=utf-8\r\n\r\n"
                            "<!DOCTYPE html><html><head><meta charset=\"UTF-8\">"
                            "<meta name=\"viewport\" content=\"width=device-width,user-scalable=no,initial-scale=1.0, maximum-scale=1.0,minimum-scale=1.0\" />"
                            "<link rel=\"shortcut icon\" href=\"https://www.right.com.cn/forum/data/attachment/album/202402/05/085631z5tmyjm5t6bmzmby.png\">"
                            "<link rel=\"icon\" href=\"https://www.right.com.cn/forum/data/attachment/album/202402/05/085631z5tmyjm5t6bmzmby.png\">"
                            "<style>"
                            ".btn-button {"
                            "  background-color: #1E90FF;"
                            "  color: white;"
                            "  padding: 12px 20px;"
                            "  border: none;"
                            "  border-radius: 4px;"
                            "  cursor: pointer;"
                            "  transition: all 0.3s ease;"
                            "  display: block;"
                            "  margin: 0 auto;" 
                            "}"
                            ".btn-button:hover {"
                            "  transform: scale(1.1);"
                            "}"
                            ".btn-success {"
                            "  background-color: #4CAF50;"
                            "  color: white;"
                            "  padding: 12px 20px;"
                            "  border: none;"
                            "  border-radius: 4px;"
                            "  cursor: pointer;"
                            "  transition: all 0.3s ease;"
                            "  display: block;"
                            "}"
                            ".btn-success:hover {"
                            "  transform: scale(1.1);"
                            "}"
                            ".btn-upsuccess {"
                            "  background-color: #4CAF50;"
                            "  color: white;"
                            "  padding: 5px 15px;"
                            "  border: none;"
                            "  border-radius: 4px;"
                            "  cursor: pointer;"
                            "  transition: all 0.3s ease;"
                            "  display: block;"
                            "}"
                            ".btn-upsuccess:hover {"
                            "  transform: scale(1.1);"
                            "}"
                             "img:hover {"
                            "  transform: scale(1.1);"
                            "}"
                            "h1 {"
                            "  text-align: center;"
                            "  font-weight: bold;"
                            "}"
                            ".centered-text {"
                            "  text-align: center;"
                            "}"
                            "th, td {"
                            "  border: 1px solid black;"
                            "  padding: 8px;"
                            "}"
                            "table {"
                            "  border-collapse: collapse;"
                            "  width: 100%;"
                            "}"
                            ".left-table {"
                            "  width: 25%; /* 左边表格宽度为25% */"
                            "  float: left; /* 让左边表格浮动在左侧 */"
                            "}"
                            ".right-table {"
                            "  width: 75%; /* 右边表格宽度为75% */"
                            "  float: right; /* 让右边表格浮动在右侧 */"
                            "}"
                            "body {"
                            "  max-width: 1200px; "/* 设置最大宽度 */
                            "  margin: 0 auto; "/* 将内容居中 */
                            "  padding: 0 20px; "/* 可选：添加左右内边距以留出更多空白 */
                            "}"
                            ".error {"
                            "border: 1px solid red;"
                            "}"
                             "#status-container {"
                             "position: fixed;"
                             "top: -50px;" /* 垂直居中 */
                             "right: 5%;" /* 距离右侧10px */
                             "transform: translateY(-50%);" /* 垂直居中 */
                             "padding: 10px 20px;"
                             "background-color: #4CAF50;" /* 绿色，可以根据需要更改颜色 */
                             "color: white;"
                             "border-radius: 5px;"
                             "transition: top 0.5s;" /* 添加渐变效果 */
                             "}"

                             "#status-container.show {"
                             "top: 50px;" /* 弹出后的位置 */
                             "}"
                             "#tag_value {"
                             "text-align: right;"
                             "}"
                             "#zerotier_status {"
                             "text-align: left;"
                             "}"
                            "</style>"
                            "<script>"
                            "function updateStatus() {"
                            "    var xhttp = new XMLHttpRequest();"
                            "    xhttp.onreadystatechange = function() {"
                            "        if (this.readyState == 4 && this.status == 200) {"
                            "            if (this.responseText.trim() == \"not running\") {"
                            "                document.getElementById(\"zerotier_status\").innerHTML = \"<font color='red'>未运行</font>\";"
                            "            } else {"
                            "                document.getElementById(\"zerotier_status\").innerHTML = \"<font color='green'>运行中</font><img src='https://www.right.com.cn/forum/data/attachment/album/202401/30/081238k459q2d5klacs8rk.gif' width='30px' alt=''>\";"
                            "            }"
                            "        }"
                            "    };"
                            "    xhttp.open(\"GET\", \"/zerotier_status\", true);"
                            "    xhttp.send();"
                            "}"
                            "setInterval(updateStatus, 1000); // 每秒更新一次状态"
                            "updateStatus(); // 页面加载时更新一次状态"
                            

                            "</script>"
                            "</head><body><div id=\"status-container\"></div>"
                            "<p><h1><img src='https://www.right.com.cn/forum/data/attachment/album/202402/05/085024luh6l6pss1yltjpj.png' width='240px' alt=''></h1></p>"
                            
                            "<table>"
                            "<tr>"
                            "<th colspan=\"3\">"
                            "<div class=\"centered-text\">zerotier是一个开源，跨平台，而且适合内网穿透异地组网的傻瓜配置虚拟 VPN LAN</div>"
                            "</th>"
                            "</tr>"
                            "<tr>"
                            "<tr> <th>运行状态</th>"
                            "<td><div><span  style=\"float: left;\" id=\"zerotier_status\"></span><span  style=\"float: right;\" id=\"tag_value\"></span></div></td>"
                            
                            "</tr>"
                            "<tr>"
                            "<th style=\"width: 30%;\">启用ZeroTier客户端</th>"
                            "<td style=\"width: 60%;\">"
                            "<div class=\"main_itoggle\">"
                            "<input style=\"float: left;\" type=\"checkbox\" id=\"zerotier_enable\" onchange=\"validateCheckbox()\" />"
                            "<input style=\"float: right;\" class=\"btn-upsuccess\" type=\"button\" name=\"zerotier_button\" value=\"更新\" onclick=\"button_zerotier_action()\" /></div>"
                            "</td>"
                            "</tr>"
                            "<tr>"
                            "<th style=\"width: 30%;\">ZeroTier Network ID</th>"
                            "<td style=\"width: 60%;\">"
                            "<input type=\"text\" class=\"input\" id=\"zerotier_id\" style=\"width: 40%\" />"
                            "<span id=\"error_message\" style=\"color: red; display: none;\">请输入有效的16位ID，包含数字和字母。</span>"
                            "</td>"
                            "</tr>"
                            "<tr><th style=\"width: 30%;\">ZeroTier Moon ID</th>"
                            "<td style=\"width: 60%;\">"
                            "<input type=\"text\" class=\"input\" id=\"zerotier_moonid\" style=\"width: 40%\"  />"
                            "<span id=\"error_idmessage\" style=\"color: red; display: none;\">请输入有效的10位ID，包含数字和字母。</span>"
                            "</td>"
                            "</tr>"
                            "<tr>"
                            "<th style=\"width: 30%;\">自动允许客户端NAT</th>"
                            "<td style=\"width: 60%;\">"
                            "<div class=\"main_itoggle\">"
                            "<input type=\"checkbox\" id=\"zerotier_nat\" onchange=\"validateCheckbox()\" />"
                            "</div>"
                            " 允许Zerotier的所有客户端访问路由器LAN下设备（需要在 Zerotier管理页面设定到LAN网段的路由表）"
                            "</td>"
                            "</tr>"
                            "<tr>"
                            "<th style=\"width: 30%;\">启用Moon服务器</th>"
                            "<td style=\"width: 60%;\">"
                            "<div class=\"main_itoggle\">"
                            "<input type=\"checkbox\" id=\"zerotiermoon_enable\" onchange=\"validateCheckbox()\" />"
                            "</div>"
                            "</td>"
                            "</tr>"
                            "<tr><th style=\"width: 30%;\">Moon服务器 IP</th>"
                            "<td style=\"width: 60%;\">"
                            "<input type=\"text\" class=\"input\" id=\"zerotiermoon_ip\" style=\"width: 40%\"  />"
                            "<span id=\"error_ipmessage\" style=\"color: red; display: none;\">请输入有效的IP地址。</span>"
                            "<br>如果没有填写，将使用Wan口IP（须为公网IP）；如写IP地址，将使用该IP（须为公网IP，不支持域名）"
                            "</td>"
                            "</tr>"
                            "<tr><th style=\"width: 30%;\">Moon服务器 ID</th>"
                            "<td style=\"width: 60%;\">"
                            "<input type=\"text\" class=\"input\" id=\"zerotiermoon_id\" style=\"width: 40%\"  readonly />"
                            "<br>服务器启用后自动生成Moon服务器的ID，在加入Moon时请使用客户端zerotier-cli orbit <该ID> <该ID>。"
                            "</td>"
                            "</tr>"
                            "<tr>"
                            "<th style=\"width: 30%;\">zerotier官网</th>"
                            "<td style=\"width: 60%;\">"
                            "<input type=\"button\" class=\"btn btn-success\" value=\"zerotier官网\" onclick=\"window.open('https://my.zerotier.com/network')\" size=\"0\">"
                            "<br>点击跳转到Zerotier官网管理平台，新建或者管理网络，并允许客户端接入访问你私人网路（新接入的节点默认不允许访问）"
                            "</td>"
                            "</tr>"
                            "</table>"
                           "<br><br><input type=\"button\" class=\"btn btn-button\" value=\"应用\" onclick=\"sendDataToServer()\">"
    "<div id=\"status\"></div>"
    "<script>"
    

        "function updateZeroTierID() {"
            "var xhr = new XMLHttpRequest();"
            "xhr.open(\"GET\", \"/zerotier_values\", true);"
            "xhr.onload = function() {"
                "if (xhr.status === 200) {"
                    "var data = xhr.responseText.split(\",\");"
                    "document.getElementById(\"zerotier_enable\").checked = (data[0].trim() === \"1\");"
                    "document.getElementById(\"zerotier_id\").value = data[1].trim();"
                    "document.getElementById(\"zerotier_moonid\").value = data[2].trim();"
                    "document.getElementById(\"zerotier_nat\").checked = (data[3].trim() === \"1\");"
                    "document.getElementById(\"zerotiermoon_enable\").checked = (data[4].trim() === \"1\");"
                    "document.getElementById(\"zerotiermoon_ip\").value = data[5].trim();"
                    "document.getElementById(\"zerotiermoon_id\").value = data[6].trim();"
                "}"
            "};"
            "xhr.send();"
        "}"

        "function sendDataToServer() {"
            "var newEnableValue = document.getElementById(\"zerotier_enable\").checked ? \"1\" : \"0\";"
            "var newIdValue = document.getElementById(\"zerotier_id\").value;"
            "var newMoonIdValue = document.getElementById(\"zerotier_moonid\").value;"
            "var newNatValue = document.getElementById(\"zerotier_nat\").checked ? \"1\" : \"0\";"
            "var newMoonEnableValue = document.getElementById(\"zerotiermoon_enable\").checked ? \"1\" : \"0\";"
            "var newMoonIpValue = document.getElementById(\"zerotiermoon_ip\").value;"
            "var xhr = new XMLHttpRequest();"
            "xhr.open(\"POST\", \"/update_zerotier_values\", true);"
            "xhr.setRequestHeader(\"Content-type\", \"application/x-www-form-urlencoded\");"
            "xhr.onload = function() {"
                "if (xhr.status === 200 && xhr.responseText.trim() === \"Values updated successfully\") {"
                    "showStatusMessage(\"保存成功\", \"#4CAF50\", 4000);" // 绿色提示，持续时间为4秒
                    //"location.reload();"
                "} else if (xhr.status === 200 && xhr.responseText.trim() === \"No Change\") {"
                "showStatusMessage(\"没有改变\", \"#1E90FF\");"
                "} else {"
                    "showStatusMessage(\"保存失败\", \"#f44336\");" // 红色提示，使用默认的持续时间（3000毫秒
               " }"
            "};"
           "xhr.send(\"new_enable_value=\" + newEnableValue + \"&new_id_value=\" + newIdValue + \"&new_moonid_value=\" + newMoonIdValue + \"&new_nat_value=\" + newNatValue + \"&new_moon_enable_value=\" + newMoonEnableValue + \"&new_moon_ip_value=\" + newMoonIpValue);"
        "}"

        "updateZeroTierID();"
        "function validateCheckbox() {"
      "var checkboxes = [\"zerotier_enable\", \"zerotier_nat\", \"zerotiermoon_enable\"];"
      "checkboxes.forEach(function(checkboxId) {"
       "var checkbox = document.getElementById(checkboxId);"
      "if (checkbox.checked) {"
        "checkbox.value = 1;"
      "} else {"
        "checkbox.value = 0;"
      "}"
       "});"
   " }"
   
   "function validateInput() {"
    "var input = this.value;"
    "var regex = /^[a-zA-Z0-9]{16}$/;"

    "if (input === '' || (input.length === 16 && regex.test(input))) {"
        "this.classList.remove('error');"
        "document.getElementById('error_message').style.display = 'none';"
   " } else {"
       " this.classList.add('error');"
       " document.getElementById('error_message').style.display = 'inline';"
    "}"
"}"
"document.getElementById('zerotier_id').addEventListener('input', validateInput);"
"function idvalidateInput() {"
    "var input = this.value;"
    "var regex = /^[a-zA-Z0-9]{10}$/;"

    "if (input === '' || (input.length === 10 && regex.test(input))) {"
        "this.classList.remove('error');"
        "document.getElementById('error_idmessage').style.display = 'none';"
   " } else {"
       " this.classList.add('error');"
       " document.getElementById('error_idmessage').style.display = 'inline';"
    "}"
"}"
"document.getElementById('zerotier_moonid').addEventListener('input', idvalidateInput);"
                            "function validateInputip() {"
                            "var input = document.getElementById('zerotiermoon_ip').value;"
                            "var ipRegex = /^(\\d{1,3}\\.){3}\\d{1,3}$/;"
                            "var errorIpMessage = document.getElementById('error_ipmessage');"
                            "if (input === '' || ipRegex.test(input)) {"
                            "this.classList.remove('error');"
                            "errorIpMessage.style.display = 'none';"
                            "} else {"
                            " this.classList.add('error');"
                            "errorIpMessage.style.display = 'inline';"
                            "}"
                            "}"
                            "document.getElementById('zerotiermoon_ip').addEventListener('input', validateInputip);"
                            "function showStatusMessage(message, color, duration) {"
                            "var statusContainer = document.getElementById(\"status-container\");"
                            "statusContainer.innerText = message;"

                            "statusContainer.style.backgroundColor = color || \"#4CAF50\";" // 默认为绿色

                            "statusContainer.classList.add(\"show\");"

                            "setTimeout(function() {"
                            "statusContainer.classList.remove(\"show\");"
                             "}, duration || 3000);" // 如果未提供持续时间，则默认为3000毫秒
                             "}"
                             "function getTagValue() {"
                            "var xhttp = new XMLHttpRequest();"
                            "xhttp.onreadystatechange = function() {"
                            "if (this.readyState == 4 && this.status == 200) {"
                            "var tagValue = this.responseText.trim();" // 获取标签的值
                            "document.getElementById(\"tag_value\").innerHTML = \"<font color='#F4A460'>\" + tagValue + \"</font>\";" // 以#000080颜色显示值
                            "}"
                            "};"
                            "xhttp.open(\"GET\", \"/get_tag_value\", true);"
                            "xhttp.send();" // 发送请求
                            "}"

                            "setInterval(getTagValue, 3000);" // 每3秒更新一次标签值
                            "getTagValue();" // 页面加载时更新一次标签值
                            "function validateCheckbox() {"
                             // 这里可以添加验证复选框的逻辑
                            "updateButton();"
                            "}"

                            "function updateButton() {"
                            "var zerotierFlag = document.getElementById(\"zerotier_enable\").checked ? 1 : 0;"
                            "var button = document.getElementsByName(\"zerotier_button\")[0];"
                            " if (zerotierFlag === 1) {"
                            " button.value = \"重启\";"
                            "button.setAttribute(\"onclick\", \"restartZerotier()\");"
                            "} else {"
                            "  button.value = \"更新\";"
                            "button.setAttribute(\"onclick\", \"updateZerotier()\");"
                            "  }"
                            "}"
                            "function updateZerotier() {"
                            " var xhr = new XMLHttpRequest();"
                            "xhr.open(\"POST\", \"/update_zerotier_tag\", true);"
                            "xhr.setRequestHeader(\"Content-type\", \"application/x-www-form-urlencoded\");"
                            "xhr.onload = function() {"
                            "if (xhr.status === 200 && xhr.responseText.trim() === \"update_ok\") {"
                               "showStatusMessage(\"执行成功\", \"#4CAF50\", 4000);" // 绿色提示，持续时间为4秒
                             "} else {"
                               "showStatusMessage(\"执行失败\", \"#f44336\");" // 红色提示，使用默认的持续时间（3000毫秒）
                            " }"
                             "};"
                             "xhr.send();"
                             " }"

                             "function restartZerotier() {"
                              "var xhr = new XMLHttpRequest();"
                               "xhr.open(\"POST\", \"/restart_zerotier\", true); "
                              "xhr.setRequestHeader(\"Content-type\", \"application/x-www-form-urlencoded\");"
                              "xhr.onload = function() {"
                              "if (xhr.status === 200 && xhr.responseText.trim() === \"restart_ok\") {"
                                 "showStatusMessage(\"执行成功\", \"#4CAF50\", 4000); "// 绿色提示，持续时间为4秒
                               "} else {"
                                "showStatusMessage(\"执行失败\", \"#f44336\");" // 红色提示，使用默认的持续时间（3000毫秒）
                               " }"
                                "};"
                               "xhr.send();"
                               "}"
                            "</script>"
                            "</body></html>";

                           // "<br><img src='https://www.right.com.cn/forum/data/attachment/album/202401/30/081238k459q2d5klacs8rk.gif' width='30px' alt=''>"

int server_fd; 
char global_argv[1024];
char *shpath = NULL;

void initializePath(char *programPath) {
    if (shpath == NULL) {
        // 从程序路径中提取程序所在的文件夹，并附加 /zerotier.sh
        char *lastSlash = strrchr(programPath, '/');
        if (lastSlash != NULL) {
            shpath = malloc(lastSlash - programPath + strlen("/zerotier.sh") + 1);
            if (shpath != NULL) {
                strncpy(shpath, programPath, lastSlash - programPath);
                shpath[lastSlash - programPath] = '\0';
                strcat(shpath, "/zerotier.sh");
            } else {
                fprintf(stderr, "\n内存分配失败，将脚本路径设为: /etc/storage/zerotier.sh\n\n");
                shpath = strdup("/etc/storage/zerotier.sh");
            }
        } else {
            fprintf(stderr, "\n无法获取程序所在的文件夹，将脚本路径设为: /etc/storage/zerotier.sh\n");
            shpath = strdup("/etc/storage/zerotier.sh");
        }
    }
}
char *allowed_ips[50];  // 假设最多允许50个IP地址访问
int allowed_ips_count = 0;

int is_ip_allowed(char *ip) {
    if (allowed_ips_count == 0) {
        return 1;  // 如果没有指定任何允许的IP地址，则允许所有IP访问
    }

    struct in_addr client_addr;
    if (inet_pton(AF_INET, ip, &client_addr) != 1) {
        return 0;  // 如果客户端IP地址无效，则不允许访问
    }

    for (int i = 0; i < allowed_ips_count; i++) {
        struct in_addr allowed_addr;
        if (inet_pton(AF_INET, allowed_ips[i], &allowed_addr) != 1) {
            continue;  // 如果允许的IP地址无效，则跳过
        }

        // 检查客户端IP地址是否在允许的网段内
        if ((client_addr.s_addr & allowed_addr.s_addr) == allowed_addr.s_addr) {
            return 1;  // IP 在允许的网段内
        }
    }
    return 0;  // IP 不在允许的网段内
}

void removeSpecialChars(char *str) {
    int i, j = 0;
    for (i = 0; str[i] != '\0'; i++) {
        if (isalnum((unsigned char)str[i])) { // 如果是字母或数字
            str[j++] = str[i];  // 将字母或数字复制到原字符串
        }
    }
    str[j] = '\0';  // 在新字符串的末尾添加字符串结束符
}


void releaseIPandPort() {
    if (server_fd != 0) {
        struct sockaddr_in address;
        socklen_t addr_len = sizeof(address);
        if (getsockname(server_fd, (struct sockaddr *)&address, &addr_len) == -1) {
            perror("获取套接字信息失败");
        } else {
            printf("\n正在释放IP：%s ：%d\n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));
        }
        close(server_fd); 
    }
}

void cleanupAndExit(int signal) {
    releaseIPandPort(); 
    printf("\n服务已关闭\n\n");
    
    char command[100];
    sprintf(command, "logger -t \"【ZeroTier_WEB】\" \"%s:服务已关闭\"", global_argv);
    system(command);
    exit(0);
}

int main(int argc, char *argv[]) {
    char *ip = NULL;
    int port = 0;
    strcpy(global_argv, argv[0]); // 将第一个命令行参数复制到global_argv中
if (argv[0][strlen(argv[0])-1] == ' ') {
    global_argv[strlen(argv[0])-1] = '\0'; // 如果第一个参数以空格结尾，去掉空格
}
    // 解析命令行参数
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-i") == 0) {
            if (i + 1 < argc) {
                ip = argv[i + 1];
                i++;
            } else {
                fprintf(stderr, "\n错误：缺少IP地址参数\n");
                return 1;
            }
        } else if (strcmp(argv[i], "-p") == 0) {
            if (i + 1 < argc) {
                port = atoi(argv[i + 1]);
                i++;
            } else {
                fprintf(stderr, "\n错误：缺少端口参数\n");
                return 1;
            }
        } else if (strcmp(argv[i], "-f") == 0) {
            if (i + 1 < argc) {
                shpath = argv[i + 1];
                i++;
            } else {
                fprintf(stderr, "\n错误：缺少路径参数\n");
                return 1;
            }
        } else if (strcmp(argv[i], "-a") == 0) {
            if (i + 1 < argc) {
                if (allowed_ips_count < 50) {  // 确保不超过允许的IP地址数量
                    allowed_ips[allowed_ips_count] = argv[i + 1];
                    allowed_ips_count++;
                    i++;
                } else {
                    fprintf(stderr, "\n错误：允许的IP地址数量超过最大50个限制\n");
                    return 1;
                }
            } else {
                fprintf(stderr, "\n错误：缺少允许访问的IP地址参数\n");
                return 1;
            }
        }
    }
    
    // 检查IP地址和端口是否有效
    if (ip == NULL || port <= 0 || port > 65535) {
        fprintf(stderr, "v1.0\n\033[1;31m用法： \033[0m\n    -i 指定监听zerotier_web的ip地址\n    -p 指定监听zerotier_web的端口\n    -f 指定zerotier的启动脚本，不填则从当前程序目录下寻找zerotier.sh，若没有则/etc/storage/zerotier.sh\n    -a 设置允许访问web界面的IP地址或网段,可多个-a 不加-a所有ip可访问  \033[0m\n\n    %s -i \033[1;32m<IP地址>\033[0m -p \033[1;34m<端口号>\033[0m -f \033[1;33m<启动脚本路径>\033[0m -a \033[1;35m<允许访问WEB的ip网段>\033[0m \n    例如：%s -i \033[1;32m192.168.2.1\033[0m -p \033[1;34m5656\033[0m -f \033[1;33m/etc/storage/zerotier.sh\033[0m -a \033[1;35m192.168.2.0\033[0m -a \033[1;35m192.168.4.78 \033[0m\n\n", argv[0], argv[0]);

        return 1;
    }

    // 创建套接字
    int server_fd;
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket 创建失败");
        return 1;
    }
 // 设置套接字选项，允许端口重用
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("设置SO_REUSEADDR选项失败");
        return 1;
    }
    // 准备地址结构体
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(ip);
    address.sin_port = htons(port);

    // 绑定套接字
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("绑定失败");
        return 1;
    }

    // 监听
    if (listen(server_fd, 30) < 0) {
        perror("监听失败");
        return 1;
    }
   // printf("\nWEB管理主页：\033[36;1mhttp://%s:%d\033[0m ", ip, port);
    int addrlen = sizeof(address);
    int new_socket;
    

    signal(SIGINT, cleanupAndExit);
    signal(SIGTERM, cleanupAndExit); 
    
    // 创建子进程
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork失败");
        return 1;
    }
  
    if (pid > 0) {
        // 父进程退出
        sleep(2);
        exit(0);
    }
     
    // 子进程继续执行

    // 输出服务启动成功
    printf("\n服务启动成功\n");
    
    char command[100];
    sprintf(command, "logger -t \"【ZeroTier_WEB】\" \"%s:服务启动成功\"", global_argv);
    system(command);
    // 设置子进程为新的会话领导
    if (setsid() < 0) {
        perror("setsid失败");
        return 1;
    }

     // 使用nvram命令获取zerotier_enable值
     char enable_value[10];
    FILE *enable_fp = popen("nvram get zerotier_enable", "r");
    fgets(enable_value, sizeof(enable_value), enable_fp);
    pclose(enable_fp);
    char ztstatus[100]; // 使用栈上的数组存储状态信息
    //memset(&ztstatus, 0, sizeof(ztstatus));
    FILE *fp = popen("pidof zerotier-one", "r");
    fgets(ztstatus, sizeof(ztstatus), fp);
    pclose(fp);
    // 清除换行符和空白字符
    char *zpos;
    if ((zpos = strchr(ztstatus, '\n')) != NULL) {
        *zpos = '\0';
    }

    int isRunning = 1;
    for (int i = 0; i < strlen(ztstatus); i++) {
        if (!isdigit(ztstatus[i])) {
            isRunning = 0;
            break;
        }
    }

   
   if (strcmp(enable_value, "1\n") == 0 && isRunning == 0) { 
    char *programPath = argv[0];
    // 确保路径已经初始化
    initializePath(programPath);
   char rezt[256];
   sprintf(rezt, "%s start &", shpath); // 开机启动
   system(rezt);
   }
     while (1) {
        // 接受连接
if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
    perror("accept");
    exit(EXIT_FAILURE);
}

      // 获取客户端IP地址
     char client_ip[INET_ADDRSTRLEN];
     struct sockaddr_in* pV4Addr = (struct sockaddr_in*)&address;
     struct in_addr ipAddr = pV4Addr->sin_addr;
     inet_ntop(AF_INET, &ipAddr, client_ip, INET_ADDRSTRLEN);
     //printf("客户端IP地址: %s\n", client_ip);
  
   
    if (!is_ip_allowed(client_ip)) {
    // 不允许连接，直接中断连接
    close(new_socket); // 关闭连接
    //return;// 不做任何其他操作，程序会继续执行后续代码
}
       // 接收请求
        char buffer[30000] = {0};
        int valread = read(new_socket, buffer, 30000);

        // 解析请求
        char method[10], path[100], protocol[20];
        sscanf(buffer, "%s %s %s", method, path, protocol);

        // 发送zerotier状态
if (strcmp(path, "/zerotier_status") == 0) {
    char zstatus[100]; // 使用栈上的数组存储状态信息
    //memset(&zstatus, 0, sizeof(zstatus));
    FILE *fp = popen("pidof zerotier-one", "r");
    fgets(zstatus, sizeof(zstatus), fp);
    pclose(fp);

    // 清除换行符和空白字符
    char *pos;
    if ((pos = strchr(zstatus, '\n')) != NULL) {
        *pos = '\0';
    }
    //printf("Status: %s\n", zstatus);

    int isRunning = 1;
    for (int i = 0; i < strlen(zstatus); i++) {
        if (!isdigit(zstatus[i])) {
            isRunning = 0;
            break;
        }
    }

    if (isRunning) {
        char response[] = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nrunning";
        send(new_socket, response, strlen(response), 0);
    } else {
        char response[] = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nnot running";
        send(new_socket, response, strlen(response), 0);
    }
    
        // 发送zerotier配置
} else if (strcmp(path, "/get_tag_value") == 0) {
// 获取zerotier_tag的值
    char tag_value[100];
    FILE *tag_fp = popen("nvram get zerotier_tag", "r");
    fgets(tag_value, sizeof(tag_value), tag_fp);
    pclose(tag_fp);

    if (strlen(tag_value) > 0) {
        char tag_response[1024];
        sprintf(tag_response, "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n%s", tag_value);
        send(new_socket, tag_response, strlen(tag_response), 0);
    }
} else if (strcmp(path, "/update_zerotier_tag") == 0) {
// 升级版本zerotier_tag
    char upzt[256];
    char *programPath = argv[0];
    // 确保路径已经初始化
    initializePath(programPath);
   sprintf(upzt, "%s update &", shpath); // 升级版本
   system(upzt);
    // 发送HTTP响应
    char upsponse[1024];
    snprintf(upsponse, sizeof(upsponse), "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nupdate_ok");
    send(new_socket, upsponse, strlen(upsponse), 0);
} else if (strcmp(path, "/restart_zerotier") == 0) {
// 重启zerotier
    char restzt[256];
    char *programPath = argv[0];
    // 确保路径已经初始化
    initializePath(programPath);
   sprintf(restzt, "%s restart &", shpath); // 重启
   system(restzt);
    // 发送HTTP响应
    char restztsponse[1024];
    snprintf(restztsponse, sizeof(restztsponse), "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nrestart_ok");
    send(new_socket, restztsponse, strlen(restztsponse), 0);
} else if (strcmp(path, "/zerotier_values") == 0) { 
    char enable_value[10];
    char id_value[100];
    char moonid_value[100];
    char nat_value[10];
    char moon_enable_value[10];
    char moon_ip_value[100];
    char moon_id_value[100];
    
    // 使用nvram命令获取zerotier_enable值
    FILE *enable_fp = popen("nvram get zerotier_enable", "r");
    fgets(enable_value, sizeof(enable_value), enable_fp);
    pclose(enable_fp);
    //获取zerotier_id的值
    FILE *id_fp = popen("nvram get zerotier_id", "r");
    fgets(id_value, sizeof(id_value), id_fp);
    pclose(id_fp);
    //获取zerotier_nat的值
    FILE *nat_fp = popen("nvram get zerotier_nat", "r");
    fgets(nat_value, sizeof(nat_value), nat_fp);
    pclose(nat_fp);
    //获取zerotier_enable_moonid的值
    FILE *moonid_fp = popen("nvram get zerotier_moonid", "r");
    fgets(moonid_value, sizeof(moonid_value), moonid_fp);
    pclose(moonid_fp);
    //获取zerotier_enable_moon的值
    FILE *moon_enable_fp = popen("nvram get zerotiermoon_enable", "r");
    fgets(moon_enable_value, sizeof(moon_enable_value), moon_enable_fp);
    pclose(moon_enable_fp);
    //获取zerotier_moon_ip的值
    FILE *moon_ip_fp = popen("nvram get zerotiermoon_ip", "r");
    fgets(moon_ip_value, sizeof(moon_ip_value), moon_ip_fp);
    pclose(moon_ip_fp);
    //获取zerotier_moon_id的值
    FILE *moon_id_fp = popen("nvram get zerotiermoon_id", "r");
    fgets(moon_id_value, sizeof(moon_id_value), moon_id_fp);
    pclose(moon_id_fp);

    // 构建HTTP响应
    char response[1024];
    snprintf(response, sizeof(response), "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n %s, %s, %s, %s, %s, %s, %s", enable_value, id_value, moonid_value, nat_value, moon_enable_value, moon_ip_value, moon_id_value);
    send(new_socket, response, strlen(response), 0);
} else if (strcmp(path, "/update_zerotier_values") == 0) {
    char new_enable_value[10];    
    char enable_value[10];
    
    char new_id_value[100]; 
    char id_value[100];
    
    char new_moonid_value[10];
    char moonid_value[100];
    
    char new_nat_value[10];
    char nat_value[10];
    
    char new_moon_enable_value[10];
    char moon_enable_value[10];
    
    char new_moon_ip_value[100];
    char moon_ip_value[100];
    
    char new_moon_id_value[100];
    char moon_id_value[100];
    

    // 解析请求数据，获取参数值
  char* data_ptr = strstr(buffer, "\r\n\r\n"); // 定位到请求数据的起始位置
 if (data_ptr) {
     data_ptr += 4; // 移动到数据的起始位置
    char* token = strtok(data_ptr, "&");
    char* new_enable_value1 = NULL;
    char* new_id_value1 = NULL;
    char* new_moonid_value1 = NULL;
    char* new_nat_value1 = NULL;
    char* new_moon_enable_value1 = NULL;
    char* new_moon_ip_value1 = NULL;
    while (token != NULL) {
        if (strncmp(token, "new_enable_value=", 17) == 0) {
            new_enable_value1 = token + 17;
        } else if (strncmp(token, "new_id_value=", 13) == 0) {
            new_id_value1 = token + 13;
        } else if (strncmp(token, "new_moonid_value=", 17) == 0) {
            new_moonid_value1 = token + 17;
        } else if (strncmp(token, "new_nat_value=", 14) == 0) {
            new_nat_value1 = token + 14;
        } else if (strncmp(token, "new_moon_enable_value=", 22) == 0) {
            new_moon_enable_value1 = token + 22;
        } else if (strncmp(token, "new_moon_ip_value=", 18) == 0) {
            new_moon_ip_value1 = token + 18;
        }
        token = strtok(NULL, "&");
    }
    
    
if (new_enable_value1) {
    strncpy(new_enable_value, new_enable_value1, 10); // 复制内容，最多复制99个字符
    new_enable_value[10] = '\0'; // 确保字符串以null结尾
} else {
    new_enable_value[0] = '\0'; // 如果new_enable_value为空，则将新字符串设置为空字符串
}

// 复制new_id_value1的内容到数组
if (new_id_value1) {
    strncpy(new_id_value, new_id_value1, 99); 
    new_id_value[99] = '\0'; 
} else {
    new_id_value[0] = '\0'; 
}

// 复制new_moonid_value1的内容到数组
if (new_moonid_value1) {
    strncpy(new_moonid_value, new_moonid_value1, 99);
    new_moonid_value[99] = '\0'; 
} else {
    new_moonid_value[0] = '\0'; 
}

// 复制new_nat_value1的内容到数组
if (new_nat_value1) {
    strncpy(new_nat_value, new_nat_value1, 10); 
    new_nat_value[10] = '\0'; 
} else {
    new_nat_value[0] = '\0'; 
}

// 复制new_moon_enable_value1的内容到数组
if (new_moon_enable_value1) {
    strncpy(new_moon_enable_value, new_moon_enable_value1, 10); 
    new_moon_enable_value[10] = '\0'; 
} else {
    new_moon_enable_value[0] = '\0'; 
}

// 复制new_id_value的内容到数组
if (new_id_value1) {
    strncpy(new_id_value, new_id_value1, 99); // 复制内容，最多复制99个字符
    new_id_value[99] = '\0'; // 确保字符串以null结尾
} else {
    new_id_value[0] = '\0'; // 如果new_id_value为空，则将新字符串设置为空字符串
}

// 复制new_moon_ip_value1的内容到数组
if (new_moon_ip_value1) {
    strncpy(new_moon_ip_value, new_moon_ip_value1, 99); 
    new_moon_ip_value[99] = '\0';
} else {
    new_moon_ip_value[0] = '\0';
}


                // 打印参数值
                //if (new_enable_value) {
                  //  printf("new_enable_value: %s\n", new_enable_value);
               // }
               // if (new_id_value) {
               //     printf("new_id_value: %s\n", new_id_value);
              //  }
 }

    // 接收客户端发送的zerotier_enable和zerotier_id的值
   // recv(new_socket, new_enable_value, sizeof(new_enable_value), 0);
   // recv(new_socket, new_id_value, sizeof(new_id_value), 0);

    // 获取当前的zerotier的值
    FILE *enable_fp = popen("nvram get zerotier_enable", "r");
    fgets(enable_value, sizeof(enable_value), enable_fp);
    pclose(enable_fp);

   FILE *id_fp = popen("nvram get zerotier_id", "r");
   fgets(id_value, sizeof(id_value), id_fp);
    pclose(id_fp);
     
   //获取zerotier_nat的值
    FILE *nat_fp = popen("nvram get zerotier_nat", "r");
    fgets(nat_value, sizeof(nat_value), nat_fp);
    pclose(nat_fp);
    //获取zerotier_enable_moonid的值
    FILE *moonid_fp = popen("nvram get zerotier_moonid", "r");
    fgets(moonid_value, sizeof(moonid_value), moonid_fp);
    pclose(moonid_fp);
    //获取zerotier_enable_moon的值
    FILE *moon_enable_fp = popen("nvram get zerotiermoon_enable", "r");
    fgets(moon_enable_value, sizeof(moon_enable_value), moon_enable_fp);
    pclose(moon_enable_fp);
    //获取zerotier_moon_ip的值
    FILE *moon_ip_fp = popen("nvram get zerotiermoon_ip", "r");
    fgets(moon_ip_value, sizeof(moon_ip_value), moon_ip_fp);
    pclose(moon_ip_fp);
 
 //去掉空格换行
    removeSpecialChars(enable_value);
    removeSpecialChars(new_enable_value);
    
   removeSpecialChars(new_id_value);
    removeSpecialChars(id_value);
    
    removeSpecialChars(new_moonid_value);
    removeSpecialChars(moonid_value);
    
   removeSpecialChars(new_nat_value);
    removeSpecialChars(nat_value);

    
   removeSpecialChars(new_moon_enable_value);
    removeSpecialChars(moon_enable_value);

    
   removeSpecialChars(new_moon_ip_value);
    removeSpecialChars(moon_ip_value);
    
    // 检查接收到的值是否与当前值不一致
    if (strcmp(new_enable_value, "0") != 0 && strcmp(new_enable_value, "1") != 0) {
        strcpy(new_enable_value, "0");
     }
    if (strcmp(new_nat_value, "0") != 0 && strcmp(new_nat_value, "1") != 0) {
        strcpy(new_nat_value, "0");
    }
    if (strcmp(new_moon_enable_value, "0") != 0 && strcmp(new_moon_enable_value, "1") != 0) {
        strcpy(new_moon_enable_value, "0");
    }
    int commandExecuted = 0; // 初始化标志变量
   if (strcmp(new_enable_value, enable_value) != 0) {
        // 使用nvram set命令更新zerotier_enable的值
       char set_enable_cmd[100];
      snprintf(set_enable_cmd, sizeof(set_enable_cmd), "nvram set zerotier_enable=%s", new_enable_value);
        system(set_enable_cmd);
        char command[255];
        const char* zerotierenable = (strcmp(new_enable_value, "1") == 0) ? "开启zerotier" : "关闭zerotier";
        sprintf(command, "logger -t \"【ZeroTier_WEB】\" \"%s:%s %s\"", global_argv, client_ip,  zerotierenable);
      system(command);
      commandExecuted = 1;
    }

   if (strcmp(new_id_value, id_value) != 0) {
      // 更新zerotier_id的值
      //time_t rawtime;
      //struct tm * timeinfo;
     // char timestr[20];
      //time(&rawtime);
      //timeinfo = localtime(&rawtime);

      //sprintf(command, "logger -t \"%s\" \"ip改变 at %s\"", timestr, timestr);
      char set_id_cmd[100];
      snprintf(set_id_cmd, sizeof(set_id_cmd), "nvram set zerotier_id=%s", new_id_value);
      
      system(set_id_cmd);
      char command[255];
      sprintf(command, "logger -t \"【ZeroTier_WEB】\" \"%s:%s 更改 ZeroTier Network ID 为：%s\"", global_argv, client_ip, new_id_value);
      system(command);
      commandExecuted = 1;
   }
   if (strcmp(new_moonid_value, moonid_value) != 0) {
        // 更新zerotier_moonid的值
       char set_enable_cmd[100];
      snprintf(set_enable_cmd, sizeof(set_enable_cmd), "nvram set zerotier_moonid=%s", new_moonid_value);
        system(set_enable_cmd);
        char command[255];
        sprintf(command, "logger -t \"【ZeroTier_WEB】\" \"%s:%s 更改 ZeroTier Moon ID 为：%s\"", global_argv, client_ip, new_moonid_value);
      system(command);
      commandExecuted = 1;
    }

   if (strcmp(new_nat_value, nat_value) != 0) {
       // 更新zerotier_nat的值
      
      char set_id_cmd[100];
      snprintf(set_id_cmd, sizeof(set_id_cmd), "nvram set zerotier_nat=%s", new_nat_value);
        system(set_id_cmd);
        char command[255];
        const char* natenable = (strcmp(new_nat_value, "1") == 0) ? "开启 zerotier NAT" : "关闭 zerotier NAT";
        sprintf(command, "logger -t \"【ZeroTier_WEB】\" \"%s:%s %s\"", global_argv, client_ip, natenable);
      system(command);
      commandExecuted = 1;
   }
   if (strcmp(new_moon_enable_value, moon_enable_value) != 0) {
        // 更新zerotiermoon_enable的值
       char set_enable_cmd[100];
      snprintf(set_enable_cmd, sizeof(set_enable_cmd), "nvram set zerotiermoon_enable=%s", new_moon_enable_value);
        system(set_enable_cmd);
        char command[255];
        const char* moonenable = (strcmp(new_moon_enable_value, "1") == 0) ? "开启 Moon服务器" : "关闭 Moon服务器";
        sprintf(command, "logger -t \"【ZeroTier_WEB】\" \"%s:%s %s\"", global_argv, client_ip, moonenable);
      system(command);
      commandExecuted = 1;
    }

   if (strcmp(new_moon_ip_value, moon_ip_value) != 0) {
        
      
      char set_id_cmd[100];
      snprintf(set_id_cmd, sizeof(set_id_cmd), "nvram set zerotiermoon_ip=%s", new_moon_ip_value);
        system(set_id_cmd);
        char command[255];
        sprintf(command, "logger -t \"【ZeroTier_WEB】\" \"%s:%s 更改 Moon服务器 IP 为：%s\"", global_argv, client_ip, new_moon_ip_value);
      system(command);
      commandExecuted = 1;
   }
   
   if (commandExecuted == 1) {
    // 保存更新的值
   system("nvram commit");
   // 获取当前程序的路径
    char *programPath = argv[0];
    // 确保路径已经初始化
    initializePath(programPath);
   char rezt[256];
   sprintf(rezt, "%s restat &", shpath); // 改变了参数进行重启
   system(rezt);
    // 发送HTTP响应
    char response[1024];
    snprintf(response, sizeof(response), "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nValues updated successfully");
    send(new_socket, response, strlen(response), 0);
    } else {
    char response[1024];
    snprintf(response, sizeof(response), "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nNo Change");
    send(new_socket, response, strlen(response), 0);
    }
} else {
    // 发送网页
    send(new_socket, html_response, strlen(html_response), 0);
}

        // 关闭套接字
        close(new_socket);
    }

    return 0;
}

