FROM node:18-alpine

# Create app directory
WORKDIR /app

# Install app dependencies
COPY package.json package-lock.json* ./
RUN npm install --production || true

# Bundle app source
COPY . .

EXPOSE 8000

# Default environment variable for local dev
ENV PORT=8000

CMD [ "node", "server.js" ]
