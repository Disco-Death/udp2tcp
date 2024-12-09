# UDP to TCP Forwarder

Этот проект представляет собой программу, которая принимает UDP-пакеты и пересылает их по TCP. Программа также ведет логирование событий в файл.

## Описание

Программа создает UDP-сокет для прослушивания входящих пакетов и TCP-сокет для отправки данных на указанный сервер. Все полученные UDP-пакеты обрабатываются и отправляются на сервер по TCP с добавлением заданного префикса.

## Функциональные возможности

- Принимает UDP-пакеты от клиентов.
- Пересылает полученные данные на указанный TCP-сервер.
- Логирует события в указанный файл.
- Поддерживает пользовательский префикс для отправляемых данных.

## Требования

- Linux (или совместимая система)
- gcc
- pthread библиотека

## Установка

1. Склонируйте репозиторий или скачайте файлы проекта.
2. Убедитесь, что у вас установлен компилятор gcc и необходимые библиотеки для работы с сокетами.
3. Откройте терминал и перейдите в директорию с проектом.
4. Выполните команду для сборки проекта:

```bash
make
```

## Использование

Запустите программу с необходимыми параметрами:

```bash
./udp_to_tcp <UDP ip:port> <TCP ip:port> <pathtolog_file> <prefix>
```

### Параметры

- <UDP ip:port> — адрес и порт, на которых программа будет слушать UDP-пакеты (например, 0.0.0.0:12345).
- <TCP ip:port> — адрес и порт TCP-сервера, на который будут отправляться данные (например, 192.168.1.10:54321).
- <path_to_log_file> — путь к файлу, в который будут записываться логи (например, a.log).
- <prefix> — строка длиной до 4 символов, которая будет добавлена перед данными при отправке по TCP.

### Пример запуска

```bash
./udp_to_tcp 0.0.0.0:12345 192.168.1.10:54321 a.log ABCD
```

## Логирование

Все события, такие как ошибки соединения и получение данных, будут записываться в указанный лог-файл.

## Сборка и запуск

Для сборки проекта используйте команду make, а для очистки скомпилированных файлов — make clean.

## Очистка

Чтобы удалить скомпилированные файлы, используйте команду:

```bash
make clean
```