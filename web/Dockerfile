FROM dc-core:latest
WORKDIR /dc-web
COPY ["package.json", "package-lock.json", "./"]
RUN DEBIAN_FRONTEND=noninteractive apt-get install -y nodejs npm
RUN npm install
COPY . .
#COPY --from=dc-core:latest /dc-core/bin/ ./bin/
#RUN chmod +x ./bin/dc_core
RUN npm run transpile
CMD npm run start
#docker run -p 2608:2608 --mount type=tmpfs,destination=/usr/temp dc-web