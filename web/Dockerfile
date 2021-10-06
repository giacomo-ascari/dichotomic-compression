FROM dc-core:latest
WORKDIR /dc-web
COPY ["package.json", "package-lock.json", "./"]
RUN DEBIAN_FRONTEND=noninteractive apt-get install -y nodejs npm
RUN npm install
COPY . .
RUN npm run transpile
CMD npm run start
#docker run -p 27001:80 --mount type=tmpfs,destination=/usr/temp dc-web