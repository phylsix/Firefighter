$(document).ready(function() {
  $('#ts').html('Updated at: ' + data.updateTime);
  var $mxx = 0;
  $('#table2mu2e tr').each(function(i, row) {
    var $row = $(row);
    if ($row.has('th').length) return;       // skip header
    if ($row.children().length < 5) return;  // skip sep row
    if ($row.has('td[rowspan]').length) $mxx = $row.find('td[rowspan]').text();
    var $ctaus = $row.children();
    var $ma = $ctaus.eq(-6).text();
    for (var i = -5; i != 0; i++) {
      var $ctau = $ctaus.eq(i).text();
      var $status = getSignalProdStatus('2mu2e', $mxx, $ma, $ctau);
      var $detail = getSignalProdDetail('2mu2e', $mxx, $ma, $ctau);
      $ctaus.eq(i).css('background-color', bkgColorfy($status));

      $ctaus.eq(i).click({detail: $detail}, function(e) {
        $('#info').html(fmtDetail(e.data.detail));
        $('#table2mu2e tr td').css('font-weight', 'normal');
        $('#table4mu tr td').css('font-weight', 'normal');
        $(this).css('font-weight', 'bold');
        e.stopPropagation();
      })
    }
  })

  $mxx = 0;
  $('#table4mu tr').each(function(i, row) {
    var $row = $(row);
    if ($row.has('th').length) return;       // skip header
    if ($row.children().length < 5) return;  // skip sep row
    if ($row.has('td[rowspan]').length) $mxx = $row.find('td[rowspan]').text();
    var $ctaus = $row.children();
    var $ma = $ctaus.eq(-6).text();
    for (var i = -5; i != 0; i++) {
      var $ctau = $ctaus.eq(i).text();
      var $status = getSignalProdStatus('4mu', $mxx, $ma, $ctau);
      var $detail = getSignalProdDetail('4mu', $mxx, $ma, $ctau);
      $ctaus.eq(i).css('background-color', bkgColorfy($status));

      $ctaus.eq(i).click({detail: $detail}, function(e) {
        $('#info').html(fmtDetail(e.data.detail));
        $('#table2mu2e tr td').css('font-weight', 'normal');
        $('#table4mu tr td').css('font-weight', 'normal');
        $(this).css('font-weight', 'bold');
        e.stopPropagation();
      })
    }
  })
})


function getUpdateTime() {
  return data.updateTime;
}

function getSignalProdStatus(channel, mxx, ma, ctau) {
  var $store = data.store;
  var $status_code = -1;
  for (var i = 0; i < $store.length; i++) {
    var $entry = $store[i];
    if (JSON.stringify($entry.identifier) ===
        JSON.stringify([channel, mxx, ma, ctau])) {
      if ($entry.jobstatus==='COMPLETED') {
        $status_code = 0;
      } else if ($entry.jobstatus==='SUBMITTED') {
        $status_code = 1;
      } else if ($entry.jobstatus==='TAPERECALL') {
        $status_code = 2;
      } else if ($entry.jobstatus==='FAILED') {
        $status_code = 3;
      }

      break;
    }
  }

  return $status_code;  // not in system
}


function getSignalProdDetail(channel, mxx, ma, ctau) {
  var $store = data.store;
  var $entry = undefined;
  for (var i = 0; i < $store.length; i++) {
    if (JSON.stringify($store[i].identifier) ===
        JSON.stringify([channel, mxx, ma, ctau])) {
        $entry = $store[i];
        return $entry;
    }
  }
  return $entry;
}

function bkgColorfy(status) {
  switch (status) {
    case 0: return 'green';
    case 1: return 'orange';
    case 2: return 'DarkCyan';
    case 3: return 'Crimson';
    case -1:
      return 'grey';
    default:
      return 'white';
  }
}


function fmtDetail(detail) {
  if (!detail) return '<b>UNKNOWN</b>';

  var res = '<ul>';
  res += '<li><b>name</b>: ' + detail.name + '</li>';
  res += '<li><b>status</b>: ' + detail.status + '</li>';
  res += '<li><b>jobStatus</b>: ' + detail.jobstatus + '</li>';
  res += '<li><b>submitDir</b>: ' + detail.submitdir + '</li>';
  res += '<li><b>lastCrabTime</b>: ' + detail.lastcrabtime + '</li>';
  res += '<li><b>lastEosTime</b>: ' + detail.lasteostime + '</li>';
  // res += '<li><b>onDisk</b>: ' + detail.ondisk + '</li>';
  res += '<li><b>ntupleFileList</b>: <ul>' +
      detail.ntuplefiles
          .map((val) => {
            return '<li>' + val + '</li>';
          })
          .join('') +
      '</ul></li>';
  res += '</ul>';
  return res;
}
