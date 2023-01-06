#!/usr/bin/ python
# -*- coding: utf-8 -*-
# github.com/cerebnismus

import random
import time

from fp.fp import FreeProxy
from fake_useragent import UserAgent
from bs4 import BeautifulSoup

from selenium import webdriver
from selenium.webdriver import Chrome
from selenium.webdriver.chrome.options import Options

from selenium.webdriver.common.by import By
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.firefox.service import Service
from selenium.common.exceptions import TimeoutException
from selenium.webdriver.support.wait import WebDriverWait
from selenium.webdriver.common.action_chains import ActionChains
from selenium.webdriver.support import expected_conditions as EC

""" reCAPTCHA v2 solver. """
import asyncio
from capmonstercloudclient import CapMonsterClient, ClientOptions
from capmonstercloudclient.requests import RecaptchaV2ProxylessRequest

"""

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


class Spoofer(object):

    def __init__(self, country_id=['US', 'GB'], rand=True, anonym=True):
        self.country_id = country_id
        self.rand = rand
        self.anonym = anonym
        self.userAgent, self.ip = self.get()

    def get(self):
        ua = UserAgent(browsers=["chrome"])
        proxy = FreeProxy(country_id=self.country_id,
                          rand=self.rand, anonym=self.anonym).get()
        ip = proxy.split("://")[1]
        return ua.random, ip


class DriverOptions(object):

    def __init__(self):

        """ 
    
        # Options for other browsers

        chrome_browser_path = "/Applications/Google Chrome.app/Contents/MacOS/Google Chrome"
        self.options.binary_location = chrome_browser_path

        firefox_browser_path = "/Applications/Firefox.app/Contents/MacOS/firefox-bin"
        self.options.binary_location = firefox_browser_path

        tor_browser_path = "/Applications/Tor Browser.app/Contents/MacOS/firefox"
        self.options.binary_location = tor_browser_path

        profile_path = os.path.join(os.path.expanduser('~'), 'Library/Application Support/TorBrowser-Data/Browser/profile.default')
        self.options.set_preference("profile", profile_path)

        self.options.set_preference("network.proxy.type", 1)
        self.options.set_preference("network.proxy.socks", "127.0.0.1")
        self.options.set_preference("network.proxy.socks_port", 9050)
        self.options.set_preference("network.proxy.socks_remote_dns", True)
        self.options.set_preference("network.proxy.socks_version", 5)
        
        """

        self.options = Options()
        
        # open in headless mode to run in background
        self.options.headless = False

        self.options.binary_location = '/Applications/Google Chrome.app/Contents/MacOS/Google Chrome'
        self.options.add_argument('--incognito')
        self.options.add_argument('--no-sandbox')
        self.options.add_argument('--single-process')
        self.options.add_argument('--start-maximized')
        self.options.add_argument('--start-fullscreen')
        self.options.add_argument('--disable-dev-shm-usage')
        self.options.add_argument('--disable-blink-features=AutomationControlled')
        self.options.add_argument('--disable-blink-features=AutomationControlled')
        
        # following options are for headless mode
        self.options.add_experimental_option('useAutomationExtension', False)
        self.options.add_experimental_option('excludeSwitches', ['enable-automation'])

        # following options reduce the RAM usage
        self.options.add_argument('--disable-application-cache')
        self.options.add_argument('--disable-extensions')
        self.options.add_argument('--disable-infobars')
        self.options.add_argument('--disable-gpu')

        self.helperSpoofer = Spoofer()
        # self.options.add_argument('user-agent={}'.format(self.helperSpoofer.userAgent))
        
        # get user-agent from list
        user_agents = ['Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.114 Safari/537.36',
                       'Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.109 Safari/537.36']

        user_agent = random.choice(user_agents)
        self.options.add_argument('user-agent={}'.format(user_agent))
        self.options.add_argument('--proxy-server=%s' % self.helperSpoofer.ip)

class WebDriver(DriverOptions):

    def __init__(self):
        DriverOptions.__init__(self)
        self.driver_instance = self.get_driver()

    def get_driver(self):

        print("""
PROXY IP  : {}
UserAgent : {}""".format(self.helperSpoofer.ip, self.helperSpoofer.userAgent))

        PROXY = self.helperSpoofer.ip
        webdriver.DesiredCapabilities.CHROME['proxy'] = {
            "httpProxy": PROXY,
            "ftpProxy": PROXY,
            "sslProxy": PROXY,
            "noProxy": None,
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
        
        # Added for validate the user agent
        # user_agent_check = driver.execute_script("return navigator.userAgent;")
        # print("UserAgent :",user_agent_check)
        
        return driver


def main():

    driver = WebDriver()
    driverinstance = driver.driver_instance
    time.sleep(2)
    # wait = WebDriverWait(driverinstance, 10)
    # driverinstance.get("https://wtfismyip.com/text")
    # time.sleep(2)
    # wtfismyip = wait.until(EC.presence_of_element_located((By.CSS_SELECTOR, 'pre')))
    # time.sleep(2)
    # print('WAN IP : ', wtfismyip.text)
    
    # for testing purpose
    # https://the-internet.herokuapp.com/login
    # https://www.google.com/recaptcha/api2/demo
    driverinstance.get("https://patrickhlauke.github.io/recaptcha/")
    
    wait = WebDriverWait(driverinstance, 10)
    wait.until(EC.presence_of_element_located((By.CSS_SELECTOR, 'iframe')))
    time.sleep(2)
    client_options = ClientOptions(api_key="c328cc9e52e4080de68534209384b916")
    cap_monster_client = CapMonsterClient(options=client_options)
    time.sleep(2)
    website_url = driverinstance.current_url
    website_key = "6Lcg7CMUAAAAANphynKgn9YAgA4tQ2KI_iqRyTwd"

    cap_monster_client = CapMonsterClient(options=client_options)
    recaptcha2request = RecaptchaV2ProxylessRequest(websiteUrl=website_url, websiteKey=website_key)
    # recaptcha2request.set_verbose(1)
    # recaptcha2request.set_invisible(True)
    # recaptcha2request.set_min_score(0.3)
    # recaptcha2request.set_page_action("verify")
    print("# Task created successfully, waiting for the response.")

    response = cap_monster_client.solve_captcha(recaptcha2request)
    # check response is not None
    if response is not None:
        print("# Response received.")

    driverinstance.execute_script("document.getElementById('g-recaptcha-response').innerHTML = '{}'".format(response
    time.sleep(2)
    print("# Response injected to secret input.")
    driverinstance.execute_script("document.getElementById('recaptcha-demo-form').submit()")
    print("# Form submitted.")
    
    time.sleep(20)
    driverinstance.quit()
    

    """ 
    username = wait.until(EC.presence_of_element_located((By.ID, 'username')))
    username.send_keys('tomsmith')
    
    password = wait.until(EC.presence_of_element_located((By.ID, 'password')))
    password.send_keys('SuperSecretPassword!')

    submit = wait.until(EC.presence_of_element_located((By.CSS_SELECTOR, 'button.radius')))
    submit.click()
    
    validate_login = wait.until(EC.presence_of_element_located((By.CSS_SELECTOR, 'div.flash.success')))
    print(validate_login.text)

    time.sleep(2)
    logout = wait.until(EC.presence_of_element_located((By.CSS_SELECTOR, 'a.button.secondary.radius')))
    logout.click()
    
    validate_logout = wait.until(EC.presence_of_element_located((By.CSS_SELECTOR, 'div.flash.success')))
    print(validate_logout.text)

    time.sleep(2)
    driverinstance.quit()
    """
    
    print("\n\tdone\n")


if __name__ == "__main__":

    # call main function repeatedly until not getting any error
    while True:
        try:
            main()
        except Exception as e:
            print(e)
            time.sleep(5)
            continue
        break
    
    # most of the time it can't get the IP address from the proxy and raise the error below
    # selenium.common.exceptions.WebDriverException: Message: unknown error: net::ERR_TUNNEL_CONNECTION_FAILED
    
    
    
    
    
'''

#!/usr/bin/ python
# -*- coding: utf-8 -*-
# github.com/cerebnismus

import random, time, os

from fp.fp import FreeProxy
from fake_useragent import UserAgent
from bs4 import BeautifulSoup

from selenium import webdriver
from selenium.webdriver import Chrome
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.firefox.service import Service
from selenium.common.exceptions import TimeoutException
from selenium.webdriver.support.wait import WebDriverWait
from selenium.webdriver.common.action_chains import ActionChains
from selenium.webdriver.support import expected_conditions as EC

# for proxy
from selenium.webdriver.common.by import By
from seleniumwire import webdriver
# A package to have a chromedriver always up-to-date.
from webdriver_manager.chrome import ChromeDriverManager

USERNAME = "cerebnismus"
PASSWORD = "Ybayv4jqs*"
ENDPOINT = "pr.oxylabs.io:7777"

entry = ('http://customer-%s-cc-tr-sessid-0111786884-sesstime-10:%s@%s' % (USERNAME, PASSWORD, ENDPOINT))
# customer-cerebnismus-cc-tr-sessid-0111786884-sesstime-10:Ybayv4jqs*@pr.oxylabs.io:7777
# http://customer-cerebnismus-cc-tr-sessid-0111786884-sesstime-10:Ybayv4jqs*@pr.oxylabs.io:7777

def chrome_proxy(user: str, password: str, endpoint: str) -> dict:
    wire_options = {
        "proxy": {
            "http": entry,
            "https": entry,
        }
    }

    return wire_options


def execute_driver():
    options = webdriver.ChromeOptions()
    options.headless = True
    proxies = chrome_proxy(USERNAME, PASSWORD, ENDPOINT)
    driver = webdriver.Chrome(
        ChromeDriverManager().install(), options=options, seleniumwire_options=proxies
    )
    try:
        driver.get("https://ip.oxylabs.io/")
        return f'\nYour IP is: {driver.find_element(By.CSS_SELECTOR, "pre").text}'
    finally:
        driver.quit()


# for reCAPTCHA v2 solver
from anticaptchaofficial.recaptchav2proxyless import *

"""

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


        # Options for other browsers on mac
        self.options.binary_location = '/Applications/Google Chrome.app/Contents/MacOS/Google Chrome'

        chrome_browser_path = "/Applications/Google Chrome.app/Contents/MacOS/Google Chrome"
        self.options.binary_location = chrome_browser_path

        firefox_browser_path = "/Applications/Firefox.app/Contents/MacOS/firefox-bin"
        self.options.binary_location = firefox_browser_path

        tor_browser_path = "/Applications/Tor Browser.app/Contents/MacOS/firefox"
        self.options.binary_location = tor_browser_path

        profile_path = os.path.join(os.path.expanduser(
            '~'), 'Library/Application Support/TorBrowser-Data/Browser/profile.default')
        self.options.set_preference("profile", profile_path)

        self.options.set_preference("network.proxy.type", 1)
        self.options.set_preference("network.proxy.socks", "127.0.0.1")
        self.options.set_preference("network.proxy.socks_port", 9050)
        self.options.set_preference("network.proxy.socks_remote_dns", True)
        self.options.set_preference("network.proxy.socks_version", 5)

"""

class Spoofer(object):

    def __init__(self, country_id=['US', 'GB'], rand=True, anonym=True):
        self.country_id = country_id
        self.rand = rand
        self.anonym = anonym
        self.userAgent, self.ip = self.get()

    def get(self):
        ua = UserAgent(browsers=["chrome"])
        proxy = FreeProxy(country_id=self.country_id,
                 timeout=1, rand=self.rand, anonym=self.anonym).get()
        ip = proxy.split("://")[1]
        return ua.random, ip

class DriverOptions(object):

    def __init__(self):

        self.options = Options()
        self.options.headless = False
        self.options.binary_location = '/Applications/Google Chrome.app/Contents/MacOS/Google Chrome'
        # self.options.binary_location = '/usr/bin/chromium'
        self.options.add_argument('--no-sandbox')
        # self.options.add_argument('--start-maximized')
        # self.options.add_argument('--start-fullscreen')
        self.options.add_argument('--single-process')
        self.options.add_argument('--disable-dev-shm-usage')
        self.options.add_argument('--incognito')
        self.options.add_argument('--disable-blink-features=AutomationControlled')
        self.options.add_experimental_option('useAutomationExtension', False)
        self.options.add_experimental_option('excludeSwitches', ['enable-automation'])
        self.options.add_argument('disable-infobars')
        self.options.add_argument('--disable-infobars')

        # following options reduce the RAM usage
        self.options.add_argument('--disable-extensions')
        self.options.add_argument('--disable-application-cache')
        self.options.add_argument('--disable-gpu')
        
        self.helperSpoofer = Spoofer()
        # self.options.add_argument('user-agent={}'.format(self.helperSpoofer.userAgent))
        
        # get user-agent from list
        #user_agents = ['Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.114 Safari/537.36',
                       # 'Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.109 Safari/537.36']

        # user_agent = random.choice(user_agents)
        # self.options.add_argument('user-agent={}'.format(user_agent))
        self.options.add_argument('--proxy-server=%s' % self.helperSpoofer.ip)
        
        
        # curl -x pr.oxylabs.io:7777 -U customer-USERNAME:PASSWORD https://ip.oxylabs.io
        
        
        
class WebDriver(DriverOptions):

    def __init__(self, path=''):
        DriverOptions.__init__(self)
        self.driver_instance = self.get_driver()

    def get_driver(self):

        print("""
PROXY IP  : {}
UserAgent : {}""".format(self.helperSpoofer.ip, self.helperSpoofer.userAgent))

        PROXY = self.helperSpoofer.ip
        webdriver.DesiredCapabilities.CHROME['proxy'] = {
            "httpProxy": PROXY,
            "ftpProxy": PROXY,
            "sslProxy": PROXY,
            "noProxy": None,
            "proxyType": "MANUAL",
            "autodetect": False
        }
        webdriver.DesiredCapabilities.CHROME['acceptSslCerts'] = True
        driver = webdriver.Chrome(options=self.options)
        driver.execute_script("Object.defineProperty(navigator, 'webdriver', {get: () => undefined})")
        driver.execute_cdp_cmd("Page.addScriptToEvaluateOnNewDocument", {
            "source":
                "const newProto = navigator.__proto__;"
                "delete newProto.webdriver;"
                "navigator.__proto__ = newProto;"
        })
        return driver



# bypass cloudflare
def bypass_cloudflare(driver):
    try:
        driver.get("https://www.cloudflare.com/cdn-cgi/trace")
        time.sleep(2)
    except Exception as e:
        print(e)
        # exit
        exit()

def main():

    # https://intoli.com/blog/making-chrome-headless-undetectable/chrome-headless-test.html
    driver = WebDriver()
    driverinstance = driver.driver_instance
    wait = WebDriverWait(driverinstance, 10)
    time.sleep(random.randint(1, 2))
    
    bypass_cloudflare(driverinstance)
    
    # WAN IP Address
    driverinstance.get("https://wtfismyip.com/json")
    wtfismyip = wait.until(EC.presence_of_element_located((By.CSS_SELECTOR, 'pre')))
    print('WAN IP : ', wtfismyip.text)

    # driver = webdriver.Chrome(ChromeDriverManager().install())
    url = "https://visa.vfsglobal.com/tur/tr/pol/login"
    page = driverinstance.get(url)
    
    _email = "oguzhan.ince@protonmail.com"
    _password = "Ybayv4jqs*"
    
    # _email_input = driverinstance.find_element(By.XPATH, "//input[@id='mat-input-0']")
    _email_input = wait.until(EC.presence_of_element_located((By.ID, 'mat-input-0')))
    _email_input.send_keys(_email)
    time.sleep(random.randint(1, 2))
    
    _password_input = wait.until(EC.presence_of_element_located((By.ID, 'mat-input-1')))
    _password_input.send_keys(_password)
    time.sleep(random.randint(1, 2))

    print("User :", _email)
    print("Pass :", _password)

    sitekey_clean = "6LdJReUUAAAAAPR1hddg-9JUC_TO13OrlKVpukHL"
    print("sitekey is: ", sitekey_clean)

    solver = recaptchaV2Proxyless()
    solver.set_verbose(1)
    solver.set_key("ed22770ac73212ee52523d36e59be6a4")
    solver.set_website_url(url)
    solver.set_website_key(sitekey_clean)

    g_response = solver.solve_and_return_solution()
    if g_response != 0:
        print("g_response" + g_response)
    else:
        print("task finished with error" + solver.error_code)

    recaptcha_response = driverinstance.find_element_by_xpath('//textarea[@id="g-recaptcha-response"]')
    driverinstance.execute_script("arguments[0].innerHTML=arguments[1]", recaptcha_response, g_response)
    
    time.sleep(2) # click on submit button
    driverinstance.find_element(By.XPATH, '//*[@class="mat-focus-indicator btn mat-btn-lg btn-block btn-brand-orange mat-stroked-button mat-button-base ng-star-inserted"]').click()

    '''
    check_mark = wait.until(EC.element_to_be_clickable((By.ID, 'mat-checkbox-1-input')))
    driverinstance.execute_script("arguments[0].click();", check_mark)
    
    check_mark = wait.until(EC.element_to_be_clickable((By.ID, 'mat-checkbox-2-input')))
    driverinstance.execute_script("arguments[0].click();", check_mark)
    
    driverinstance.find_element(By.XPATH, '//*[@class="mat-focus-indicator btn mat-btn-lg btn-brand-orange d-none d-lg-inline-block position-absolute top-n3 right-0 z-index-999 mat-raised-button mat-button-base"]').click()

    check_mark = wait.until(EC.element_to_be_clickable((By.ID,'mat-checkbox-3-input')))
    driverinstance.execute_script("arguments[0].click();", check_mark)

    '''


    time.sleep(20)
    driverinstance.quit()
    print("\n\tdone\n")


if __name__ == "__main__":

    while True:
        try: # call main function repeatedly until not getting any error
            main()
        except Exception as e:
            print(e)
            time.sleep(5)
            continue
        break

    # most of the time it can't get the IP address from the proxy and raise the error below
    # selenium.common.exceptions.WebDriverException: Message: unknown error: net::ERR_TUNNEL_CONNECTION_FAILED

'''