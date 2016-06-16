// Pebble.addEventListener('ready', createMatch)
var keys = require('message_keys')
  , matchId = null

Pebble.addEventListener('appmessage', function (e) {

  var dict = e.payload

  if (e.payload['MATCH_START']) {
    createMatch(e.payload['MATCH_START'])
  }

  if (e.payload['CONFIG_CHANGE']) {
    updateMatch(e.payload['CONFIG_CHANGE'])
  }

  if (e.payload['MATCH_EVENT']) {
    handleMatchEvent(e.payload['MATCH_EVENT'], e.payload['TIMESTAMP'])
  }

})

function createMatch (config) {

  var method = 'POST'
    , url = 'http://scoresmith-api.herokuapp.com/match/tennis'
    , request = new XMLHttpRequest()

  request.onload = function () {
    if (this.status === 201) {
      var match = JSON.parse(this.responseText)
      console.log(JSON.stringify(match, null, 2))
      matchId = match.shortId
      Pebble.showSimpleNotificationOnPebble('Live Scoreboard', 'Go to scrs.io/' + match.shortId + ' in a web browser to see a live scoreboard of this match.');
    } else {
      console.log('error creating match', this.responseText)
    }
  }

  request.open(method, url)
  request.setRequestHeader('Content-Type', 'application/json')
  request.send(JSON.stringify(
    { numSets: config[0]
    , firstServer: config[1]
    , tieBreaks: config[2] === 0
    , finalSet: intToFinalSet(config[3])
    }))

}

function updateMatch (config) {

  var method = 'PATCH'
    , url = 'http://scoresmith-api.herokuapp.com/match/' + matchId
    , request = new XMLHttpRequest()

  request.onload = function () {
    if (this.status === 200) {
      console.log('match settings updated')
    } else {
      console.log('error updating match', this.responseText)
    }
  }

  request.open(method, url)
  request.setRequestHeader('Content-Type', 'application/json')
  request.send(JSON.stringify(
    { numSets: config[0]
    , firstServer: config[1]
    , tieBreaks: config[2] === 0
    , finalSet: intToFinalSet(config[3])
    }))

}

function addMatchEvent (scorer, timestamp) {
  var method = 'POST'
    , url = 'http://scoresmith-api.herokuapp.com/match/' + matchId + '/event'
    , request = new XMLHttpRequest()

  request.onload = function () {
    if (this.status === 201) {
      console.log('match event created')
    } else {
      console.log('error posting match event', this.responseText)
    }
  }

  request.open(method, url)
  request.setRequestHeader('Content-Type', 'application/json')
  request.send(JSON.stringify({ timestamp: timestamp.toISOString(), scorer: scorer }))
}


function rewindMatch () {
  var method = 'POST'
    , url = 'http://scoresmith-api.herokuapp.com/match/' + matchId + '/rewind'
    , request = new XMLHttpRequest()

  request.onload = function () {
    if (this.status === 200) {
      console.log('match event rewound')
    } else {
      console.log('error rewinding event', this.responseText)
    }
  }

  request.open(method, url)
  request.send()
}

function handleMatchEvent (scorer, timestamp) {
  if (!scorer || !timestamp) return rewindMatch()
  addMatchEvent(scorer, new Date(1000 * timestamp))
}

function intToFinalSet (n) {
  switch (n) {
    case 0: return 'TIE_BREAK_SIX_ALL'
    case 1: return 'NO_TIE_BREAK'
    case 2: return 'CHAMPIONSHIP_TIE_BREAK'
    default: return 'TIE_BREAK_SIX_ALL'
  }
}
