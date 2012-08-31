function create_activities_page(timeline){
	//console.log(timeline);
	
	var people = get_people_from_results(timeline);
	var peopleLatestTweet = get_people_latest_tweet(people,timeline);
	//var activities = get_activities_from_results(timeline);
	var activities = all_activities;
	var columns = 3;
	
	$.each(activities,function(index,activity){
		
		if (index%columns==0){
			//console.log("index: "+index);						
			$('#boxes-container').append('<div class="row-fluid boxes-row-'+(index/columns)+'">');
		}

		$('#boxes-container .row-fluid.boxes-row-'+Math.floor(index/columns)).append(build_activity_box(activity));	
	//	console.log(activity);
		
		//add people whose latest tweet is in this activity
		var activityPeople = peopleLatestTweet.filter(function (item){
				 return item.tweet.entities.hashtags.filter(function (hashtag){
					return hashtag.text===activity
				}).length > 0
			});
		

		$('.box.'+activity).append(populate_activity_box(activity,activityPeople));	
	});
}

function build_activity_box(activity){
	var type = get_activity_type(activity);
	//console.log(type);
	var box ='<div class="span4 activity box-'+type+' box '+activity+' well"><h2>'+activity+'</h2></div><!--/span-->';
	return box;
}

function populate_activity_box(activity,activityPeople){
	
	var person_content = '';	
	$.each(activityPeople, function(k,v){
		var userimage = "http://api.twitter.com/1/users/profile_image/"+v.name+".jpg";
		person_content += '<p class="span4" > <a href="#" class=""> <img src="'+ userimage +'" alt=""></a>'+v.name+'</p>';
		//person_content += '<span class="span4">'+v.name+'</span>';
	});

	var box_content = '<div class="row-fluid activity_details"> '
					+	person_content
					+'</div><!--/row-->';
	
	return box_content;
}

function get_activity_tweets(activity, timeline){
	var tweets = timeline.filter(function (result) { 
						return result.entities.hashtags.filter(function (hashtag){
							return hashtag.text===activity
						}).length > 0;
					});
						
	return tweets;
}



function get_activity_type(activity){
	if ($.inArray(activity,danger_activities)>-1) {return "danger";}
	else if ($.inArray(activity,warning_activities)>-1) {return "warning";}
	else if ($.inArray(activity,success_activities)>-1) {return "success";}
	else return "info";
}