FROM python:3.9

WORKDIR /code

#COPY requirements.txt .
# requirements in this format:
# Flask==1.1.1
# Something==1.1.2
#RUN pip install -r requirements.txt

COPY src/ .

CMD [ "python", "./main.py" ]

# docker build . -t dichotomic-compression-py
# docker run -i dichotomic-compression-py -i