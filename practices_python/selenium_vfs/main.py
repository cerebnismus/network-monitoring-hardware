#!/usr/local/bin/python3
# -*- coding: utf-8 -*-
#  -*- cerebnismus -*-
 
import random, time, os
from selenium import webdriver
from selenium.webdriver import Chrome
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common.by import By
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.firefox.service import Service
from selenium.common.exceptions import NoSuchElementException
from selenium.webdriver.support.wait import WebDriverWait
from selenium.webdriver.common.action_chains import ActionChains
from selenium.webdriver.support import expected_conditions as EC
from anticaptchaofficial.recaptchav2proxyless import *

didsoft_credentials = "username:password"
didsoft_proxy = "ip:port"

PROXY_HOST = ' rotating proxy or host '  
PROXY_PORT = 1234
PROXY_USER = ' enter username here' 
PROXY_PASS = ' enter password here '


# paid partners
# https://anti-captcha.com/ - Expiring 
# https://my.didsoft.com - Expiring 2023-01-17

"""
class Spoofer(object):

    def __init__(self, country_id=['US', 'GB'], rand=True, anonym=True):
        self.country_id = country_id
        self.rand = rand
        self.anonym = anonym
        self.userAgent, self.ip = self.get()

    def get(self):
        ua = UserAgent()
        proxy = FreeProxy(country_id=self.country_id,
                          rand=self.rand, anonym=self.anonym).get()
        ip = proxy.split("://")[1]
        return ua.random, ip


    # Edit this on fp.py for [from fp.fp import FreeProxy]

    def __website(self, repeat):
        if repeat:
            return "https://www.sslproxies.org"
        elif self.country_id == ['US']:
            return 'https://www.us-proxy.org'
        elif self.country_id == ['GB']:
            return 'https://www.sslproxies.org'
        else:
            return 'https://www.sslproxies.org'

"""

#USERNAME = ""
#PASSWORD = ""
#ENDPOINT = "pr.oxylabs.io:7777"

# entry = ('https://customer-%s:%s@%s' % (USERNAME, PASSWORD, ENDPOINT))
# curl -x pr.oxylabs.io:7777 -U customer-USERNAME:PASSWORD https://ip.oxylabs.io
# https://www.cloudflare.com/cdn-cgi/trace

'''
def chrome_proxy(user: str, password: str, endpoint: str) -> dict:
    wire_options = {
        "proxy": {
            # "http": entry,
            "https": entry,
        }
    }

    return wire_options
'''


class DriverOptions(object):

    def __init__(self):

        self.options = Options()
        self.options.headless = False
        # self.options.binary_location = '/Applications/Chrome.app/Contents/MacOS/Google Chrome'
        self.options.add_argument('--no-sandbox')
        self.options.add_argument('--start-maximized')
        # self.options.add_argument('--start-fullscreen')
        self.options.add_argument('--single-process')
        self.options.add_argument('--disable-dev-shm-usage')
        self.options.add_argument('--incognito')
        self.options.add_argument('--disable-blink-features=AutomationControlled')
        self.options.add_experimental_option('useAutomationExtension', False)
        self.options.add_experimental_option('excludeSwitches', ['enable-automation'])
        self.options.add_argument('disable-infobars')
        self.options.add_argument('--disable-infobars')

        #self.options.add_argument('--profile-directory=1')

        # following options reduce the RAM usage
        self.options.add_argument('--disable-extensions')
        self.options.add_argument('--disable-application-cache')
        # self.options.add_argument('--disable-gpu')

        # ua = UserAgent()
        # useragentz = ua.random
        # self.options.add_argument('user-agent="Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/16.2 Safari/605.1.15"')
        # self.options.add_argument('--proxy-server=%s' % self.helperSpoofer.ip)
        
        self.options.add_argument('--proxy-server=%s' % didsoft_proxy)
        
        # oguzhan.ince.protonmail.com:p5ckpb@gate2.proxyfuel.com:2000
        # self.options.add_argument('--proxy-server=%s:%s@%s:%s' % (PROXY_USER, PROXY_PASS, PROXY_HOST, PROXY_PORT))

class WebDriver(DriverOptions):

    def __init__(self, path=''):
        DriverOptions.__init__(self)
        self.driver_instance = self.get_driver()

    def get_driver(self):
        
        '''
        print("""
PROXY IP  : {}
UserAgent : {}""".format(self.helperSpoofer.ip, self.helperSpoofer.userAgent))
        '''

        webdriver.DesiredCapabilities.CHROME['proxy'] = {
            "httpProxy": didsoft_proxy,
            "ftpProxy": didsoft_proxy,
            "sslProxy": didsoft_proxy,
            # "noProxy": None,
            "proxyType": "MANUAL",
            "autodetect": False
        } 
  

        webdriver.DesiredCapabilities.CHROME['acceptSslCerts'] = True

        # If you want to use geckodriver then getting error like below
        # Message: Unable to find a matching set of capabilities
        # service = Service('/usr/local/bin/geckodriver')
        # driver = webdriver.Chrome(service=service, options=self.options)

        driver = webdriver.Chrome(options=self.options)
        driver.execute_script(
            "Object.defineProperty(navigator, 'webdriver', {get: () => undefined})")
        driver.execute_cdp_cmd("Page.addScriptToEvaluateOnNewDocument", {
            "source":
                "const newProto = navigator.__proto__;"
                "delete newProto.webdriver;"
                "navigator.__proto__ = newProto;"
        })

        return driver


def main():
    driver = WebDriver()
    driverinstance = driver.driver_instance
    wait = WebDriverWait(driverinstance, 38) # seconds

    driverinstance.get("https://wtfismyip.com/json")
    wtfismyip = wait.until(EC.presence_of_element_located((By.CSS_SELECTOR, 'pre')))
    print(wtfismyip.text)
    
    driverinstance.get("https://visa.vfsglobal.com/tur/tr/pol/login")

    _email = " Enter your email here "
    _password = " Enter your password here "
    
    _email_input = wait.until(EC.presence_of_element_located((By.ID, 'mat-input-0')))
    _email_input.send_keys(_email)
    time.sleep(random.randint(2, 3))
    
    _password_input =wait.until(EC.presence_of_element_located((By.ID, 'mat-input-1')))
    _password_input.send_keys(_password)
    time.sleep(random.randint(2, 3))

    # wait button to be clickable id=onetrust-reject-all-handler and click
    wait.until(EC.element_to_be_clickable((By.ID, 'onetrust-reject-all-handler'))).click()
    time.sleep(random.randint(2, 3))

    sitekey_clean = "6LdJReUUAAAAAPR1hddg-9JUC_TO13OrlKVpukHL"
    print("sitekey is: ", sitekey_clean)

    solver = recaptchaV2Proxyless()
    solver.set_verbose(1)
    solver.set_key("ed22770ac73212ee52523d36e59be6a4")
    solver.set_website_url("https://visa.vfsglobal.com/tur/tr/pol/login")
    solver.set_website_key(sitekey_clean)
    print("anti-captcha account balance: " + str(solver.get_balance()))
    
    g_response = solver.solve_and_return_solution()
    if g_response != 0:
        # print("g_response" + g_response)
        time.sleep(0.5)
    else:
        print("task finished with error" + solver.error_code)

    # eger bi hata olustu bi ara tekrar deneyiniz diye bir mesaj cikarsa
    # user account 2 saat suspend oluyor !!!

    time.sleep(random.randint(3, 4))
    driverinstance.execute_script('var element=document.getElementById("g-recaptcha-response"); element.style.display=""; element.style.visibility="visible"; element.style.height="auto"; element.style.position="absolute"; element.style.zIndex="99";')
    driverinstance.execute_script("""document.getElementById("g-recaptcha-response").innerHTML = arguments[0];""", g_response)
    driverinstance.execute_script('var element=document.getElementById("g-recaptcha-response"); element.style.display="none";')

    '''
    driverinstance.execute_script('document.querySelector("body > app-root > div > app-login > section > div > div > mat-card > form").classList.remove("ng-invalid");')
    driverinstance.execute_script('document.querySelector("body > app-root > div > app-login > section > div > div > mat-card > form").classList.remove("ng-touched");')
    driverinstance.execute_script('document.querySelector("body > app-root > div > app-login > section > div > div > mat-card > form").classList.remove("ng-dirty");')

    driverinstance.execute_script('document.querySelector("body > app-root > div > app-login > section > div > div > mat-card > form").classList.add("ng-touched");')
    driverinstance.execute_script('document.querySelector("body > app-root > div > app-login > section > div > div > mat-card > form").classList.add("ng-dirty");')
    driverinstance.execute_script('document.querySelector("body > app-root > div > app-login > section > div > div > mat-card > form").classList.add("ng-valid");')
    '''
    
    # https://discourse.openbullet.dev/t/guide-recaptcha-v2-bypass-and-callbacks/3252 #BYSOFTEDITION :D
    driverinstance.execute_script("___grecaptcha_cfg.clients[0].B.B.callback('onSuccessCallback(e){this.ngZone.run(()=>this.recaptchaSuccess.emit(e))}');")
    
    time.sleep(1)
    driverinstance.find_element(By.XPATH, '//*[@class="mat-focus-indicator btn mat-btn-lg btn-block btn-brand-orange mat-stroked-button mat-button-base ng-star-inserted"]').click()  
    

    time.sleep(4)
    check_mark = wait.until(EC.element_to_be_clickable((By.ID, 'mat-checkbox-1-input')))
    
    time.sleep(30) # site has slow page loading
    driverinstance.execute_script("arguments[0].click();", check_mark)
    
    time.sleep(4)
    check_mark = wait.until(EC.element_to_be_clickable((By.ID, 'mat-checkbox-2-input')))
    driverinstance.execute_script("arguments[0].click();", check_mark)
    
    
    time.sleep(4)
    wait.until(EC.element_to_be_clickable((By.XPATH, '//*[@class="mat-focus-indicator btn mat-btn-lg btn-brand-orange d-none d-lg-inline-block position-absolute top-n3 right-0 z-index-999 mat-raised-button mat-button-base"]'))).click()
    # driverinstance.find_element(By.XPATH, '//*[@class="mat-focus-indicator btn mat-btn-lg btn-brand-orange d-none d-lg-inline-block position-absolute top-n3 right-0 z-index-999 mat-raised-button mat-button-base"]').click()

    time.sleep(4)
    check_mark = wait.until(EC.element_to_be_clickable((By.ID,'mat-checkbox-3-input')))
    driverinstance.execute_script("arguments[0].click();", check_mark)

    time.sleep(4)
    driverinstance.find_element(By.XPATH, '//*[@class="mat-focus-indicator btn mat-btn-lg btn-block btn-brand-orange mat-stroked-button mat-button-base"]').click()
    
    # until now, timeout = 6min
    
    # User Information
    _name = ""
    _surname = ""
    _type = ""
    _state = "TUR"
    _passport = ""
    _sex = "Male"
    _nationality = "TUR"
    _id_number = ""
    _date_of_birth = "25.05.1999"
    _place_of_birth = ""
    _issued_by = ""
    _date_of_issue = "31.08.2021"
    _date_of_expiry = "31.08.2031"
    _phone = ""
        
    time.sleep(2)
    _name_input = wait.until(EC.presence_of_element_located((By.ID, 'mat-input-2')))
    _name_input.send_keys(_name)
    
    time.sleep(2)
    _surname_input = wait.until(EC.presence_of_element_located((By.ID, 'mat-input-3')))
    _surname_input.send_keys(_surname)
    

    time.sleep(2)
    driverinstance.find_element(By.XPATH, '//*[@class="mat-select-value ng-tns-c85-20"]').click()  

    time.sleep(2)
    driverinstance.find_element(By.XPATH, '//*[@id="mat-option-1"]').click()
    
    
    time.sleep(2)
    driverinstance.find_element(By.XPATH, '//*[@class="mat-select-value ng-tns-c85-22"]').click()  

    time.sleep(2)
    driverinstance.find_element(By.XPATH, '//*[@id="mat-option-391"]').click()
    
    
    _passport_input = wait.until(EC.presence_of_element_located((By.ID, 'mat-input-4')))
    _passport_input.send_keys(_passport)
    
    _countrycode_input = wait.until(EC.presence_of_element_located((By.ID, 'mat-input-5')))
    _countrycode_input.send_keys('90')
    
    _phone_input = wait.until(EC.presence_of_element_located((By.ID, 'mat-input-6')))
    _phone_input.send_keys(_phone)
    
    _emailaddr_input = wait.until(EC.presence_of_element_located((By.ID, 'mat-input-7')))
    _emailaddr_input.send_keys(_email)
    
    time.sleep(2)
    driverinstance.find_element(By.XPATH, '//*[@class="mat-focus-indicator mat-stroked-button mat-button-base btn btn-block btn-brand-orange mat-btn-lg"]').click()


    time.sleep(4) # site has slow page loading
    
    time.sleep(2)
    driverinstance.find_element(By.XPATH, '//*[@class="mat-select-value ng-tns-c85-28"]').click()  
    driverinstance.find_element(By.XPATH, '//*[@id="mat-option-455"]').click()

    time.sleep(4)
    driverinstance.find_element(By.XPATH, '//*[@class="mat-select-value ng-tns-c85-30"]').click()  
    driverinstance.find_element(By.XPATH, '//*[@id="mat-option-459"]').click()

    # If class="errorMessage" red text appears, login again and try again, else continue
    
    try:
        driverinstance.find_elements(By.CLASS_NAME, "errorMessage ng-star-inserted")
        print("Not found!")
        time.sleep(6)
        # enf of main function

    # selenium.common.exceptions.NoSuchElementException
    except NoSuchElementException:

        print("Found a slot!")
        for sound in range(600):  # play sound to notify the end of the process for loop
            os.system('afplay /System/Library/Sounds/Ping.aiff')
            time.sleep(1)

    raise Exception("Try again...")


if __name__ == "__main__":

    while True:
        try:
            main()
        except Exception as e:
            print(e)
            continue
        break
    
# most of the time it can't get the IP address from the proxy and raise the error below
# selenium.common.exceptions.WebDriverException: Message: unknown error: net::ERR_TUNNEL_CONNECTION_FAILED